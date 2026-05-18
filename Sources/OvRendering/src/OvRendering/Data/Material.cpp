/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <ranges>

#include <tracy/Tracy.hpp>

#include <OvDebug/Assertion.h>
#include <OvDebug/Logger.h>

#include <OvRendering/Data/Material.h>
#include <OvRendering/HAL/UniformBuffer.h>
#include <OvRendering/HAL/TextureHandle.h>
#include <OvRendering/Resources/Texture.h>

#include <OvTools/Utils/OptRef.h>

namespace
{
	template<typename T>
	inline void HashCombine(std::size_t& seed, const T& value)
	{
		// A large odd constant derived from the golden ratio (64-bit variant).
		// It helps distribute bits more uniformly and reduce clustering/collisions.
		constexpr std::size_t kHashCombineConstant = 0x9e3779b97f4a7c15ULL;

		seed ^= std::hash<T>{}(value)
			+ kHashCombineConstant 
			// Bit mixing. They spread entropy from earlier values so order matters
			// and nearby hashes don’t collapse into similar outputs.
			+ (seed << 6)
			+ (seed >> 2);
	}

	OvRendering::Data::MaterialPropertyType UniformToPropertyValue(const std::any& p_uniformValue)
	{
		using namespace OvMaths;
		using namespace OvRendering;

		auto as = [&]<typename T>() -> std::optional<T> {
			return
				p_uniformValue.type() == typeid(T) ?
				std::optional<T>{std::any_cast<T>(p_uniformValue)} :
				std::nullopt;
		};

		if (auto value = as.operator()<bool>()) return *value;
		if (auto value = as.operator()<int>()) return *value;
		if (auto value = as.operator()<float>()) return *value;
		if (auto value = as.operator()<FVector2>()) return *value;
		if (auto value = as.operator()<FVector3>()) return *value;
		if (auto value = as.operator()<FVector4>()) return *value;
		if (auto value = as.operator()<FMatrix3>()) return *value;
		if (auto value = as.operator()<FMatrix4>()) return *value;
		if (auto value = as.operator()<HAL::TextureHandle*>()) return *value;
		if (auto value = as.operator()<Resources::Texture*>()) return *value;

		return std::monostate{};
	}

	void BindTexture(
		OvRendering::HAL::ShaderProgram& p_shader,
		const std::string& p_uniformName,
		OvRendering::HAL::TextureHandle* p_texture,
		OvRendering::HAL::TextureHandle* p_fallback,
		uint32_t p_textureSlot
	)
	{
		if (auto target = p_texture ? p_texture : p_fallback)
		{
			target->Bind(p_textureSlot);
			p_shader.SetUniform<int>(p_uniformName, p_textureSlot);
		}
	}
}

OvRendering::Data::Material::Material(OvRendering::Resources::Shader* p_shader)
{
	SetShader(p_shader);
}

void OvRendering::Data::Material::SetShader(OvRendering::Resources::Shader* p_shader)
{
	m_shader = p_shader;

	if (m_shader)
	{
		m_properties.clear();
		UpdateProperties();
	}
	else
	{
		m_properties.clear();
		InvalidatePropertySignature();
	}
}

OvTools::Utils::OptRef<OvRendering::HAL::ShaderProgram> OvRendering::Data::Material::GetVariant(
	std::optional<const std::string_view> p_pass,
	OvTools::Utils::OptRef<const Data::FeatureSet> p_override
) const
{
	if (m_shader)
	{
		return m_shader->GetVariant(
			p_pass,
			p_override.value_or(m_features)
		);
	}
	
	return std::nullopt;
}

void OvRendering::Data::Material::UpdateProperties()
{
	InvalidatePropertySignature();

	// Collect all uniform names currently used by the shader
	std::unordered_set<std::string> usedUniforms;

	auto variants_view = m_shader->GetVariants()
		| std::views::values
		| std::views::join
		| std::views::values;

	for (const auto& variant : variants_view)
	{
		for (const auto& [name, uniformInfo] : variant->GetUniforms())
		{
			usedUniforms.insert(name);

			if (!m_properties.contains(name))
			{
				m_properties.emplace(name, MaterialProperty{
					.value = UniformToPropertyValue(uniformInfo.defaultValue),
					.singleUse = false
				});
			}
		}
	}

	std::erase_if(m_properties, [&usedUniforms](const auto& property) {
		return !usedUniforms.contains(property.first);
	});
}

OvRendering::Data::MaterialSignatureSet OvRendering::Data::Material::Bind(
	std::optional<const std::string_view> p_pass,
	OvTools::Utils::OptRef<const Data::FeatureSet> p_featureSetOverride,
	HAL::Texture* p_emptyTexture2D,
	HAL::Texture* p_emptyTextureCube,
	std::optional<MaterialSignatureSet> p_previousMaterialSignature
)
{
	ZoneScoped;

	OVASSERT(IsValid(), "Attempting to bind an invalid material.");

	auto& program = m_shader->GetVariant(
		p_pass,
		p_featureSetOverride.value_or(m_features)
	);

	const auto signature = CalculateSignature(
		program,
		p_emptyTexture2D,
		p_emptyTextureCube
	);

	if (!p_previousMaterialSignature || signature.bindSignature != p_previousMaterialSignature->bindSignature)
	{
		program.Bind();
	}

	m_programInUse = program;

	return signature;
}

void OvRendering::Data::Material::UploadProperties(
	bool uploadStableProperties,
	bool uploadSingleUseProperties,
	HAL::Texture* p_emptyTexture2D,
	HAL::Texture* p_emptyTextureCube
)
{
	ZoneScoped;

	OVASSERT(m_programInUse.has_value(), "Cannot upload properties before binding");

	using namespace OvMaths;
	using enum OvRendering::Settings::EUniformType;

	auto& program = m_programInUse.value();

	for (auto& [name, prop] : m_properties)
	{
		if (!uploadStableProperties && !prop.singleUse) continue;
		if (!uploadSingleUseProperties && prop.singleUse) continue;

		const auto uniformData = program.GetUniformInfo(name);

		// Skip this property if the current program isn't using its associated uniform
		if (!uniformData)
		{
			continue;
		}

		auto& value = prop.value;
		auto uniformType = uniformData->type;

		// Iterating over the properties to set them in the shader.
		// This could have been cleaner with a visitor, but the performance impact
		// is not worth it. This is a critical path in the rendering pipeline.

		if (uniformType == BOOL)
		{
			program.SetUniform<int>(name, static_cast<int>(std::get<bool>(value)));
		}
		else if (uniformType == INT)
		{
			program.SetUniform<int>(name, std::get<int>(value));
		}
		else if (uniformType == FLOAT)
		{
			program.SetUniform<float>(name, std::get<float>(value));
		}
		else if (uniformType == FLOAT_VEC2)
		{
			program.SetUniform<FVector2>(name, std::get<FVector2>(value));
		}
		else if (uniformType == FLOAT_VEC3)
		{
			program.SetUniform<FVector3>(name, std::get<FVector3>(value));
		}
		else if (uniformType == FLOAT_VEC4)
		{
			program.SetUniform<FVector4>(name, std::get<FVector4>(value));
		}
		else if (uniformType == FLOAT_MAT3)
		{
			program.SetUniform<FMatrix3>(name, std::get<FMatrix3>(value));
		}
		else if (uniformType == FLOAT_MAT4)
		{
			program.SetUniform<FMatrix4>(name, std::get<FMatrix4>(value));
		}
		else if (uniformType == SAMPLER_2D || uniformType == SAMPLER_CUBE)
		{
			HAL::TextureHandle* handle = nullptr;
			if (auto textureHandle = std::get_if<HAL::TextureHandle*>(&value))
			{
				handle = *textureHandle;
			}
			else if (auto texture = std::get_if<Resources::Texture*>(&value))
			{
				if (*texture != nullptr)
				{
					handle = &(*texture)->GetTexture();
				}
			}
			BindTexture(program, name, handle, uniformType == SAMPLER_2D ? p_emptyTexture2D : p_emptyTextureCube, uniformData->textureSlot);
		}

		if (prop.singleUse)
		{
			value = UniformToPropertyValue(uniformData->defaultValue);
		}
	}
}

void OvRendering::Data::Material::Unbind(bool p_resetBoundProgram)
{
	ZoneScoped;

	OVASSERT(IsValid(), "Attempting to unbind an invalid material.");
	OVASSERT(m_programInUse.has_value(), "Attempting to unbind a material that hasn't been bound");

	if (p_resetBoundProgram)
	{
		m_programInUse->Unbind();
	}

	m_programInUse.reset();
}

bool OvRendering::Data::Material::HasProperty(const std::string& p_name) const
{
	OVASSERT(IsValid(), "Attempting to call HasProperty on an invalid material.");
	return m_properties.contains(p_name);
}

void OvRendering::Data::Material::SetProperty(const std::string p_name, const MaterialPropertyType& p_value, bool p_singleUse)
{
	OVASSERT(IsValid(), "Attempting to SetProperty on an invalid material.");
	OVASSERT(HasProperty(p_name), "Attempting to SetProperty on a non-existing property.");

	m_properties[p_name] = MaterialProperty{
		p_value,
		p_singleUse
	};

	if (p_singleUse)
	{
		++m_singleUsePropertySignatureVersion;
	}
	else
	{
		++m_stablePropertySignatureVersion;
	}
}

bool OvRendering::Data::Material::TrySetProperty(const std::string& p_name, const MaterialPropertyType& p_value, bool p_singleUse)
{
	if (HasProperty(p_name))
	{
		SetProperty(p_name, p_value, p_singleUse);
		return true;
	}

	return false;
}

OvTools::Utils::OptRef<const OvRendering::Data::MaterialProperty> OvRendering::Data::Material::GetProperty(const std::string p_key) const
{
	OVASSERT(IsValid(), "Attempting to GetProperty on an invalid material.");

	if (m_properties.find(p_key) != m_properties.end())
	{
		return m_properties.at(p_key);
	}

	return std::nullopt;
}

OvRendering::Resources::Shader*& OvRendering::Data::Material::GetShader()
{
	return m_shader;
}

bool OvRendering::Data::Material::HasShader() const
{
	return m_shader;
}

bool OvRendering::Data::Material::IsValid() const
{
	return HasShader();
}

void OvRendering::Data::Material::SetOrthographicSupport(bool p_supportOrthographic)
{
	m_supportOrthographic = p_supportOrthographic;
}

void OvRendering::Data::Material::SetPerspectiveSupport(bool p_supportPerspective)
{
	m_supportPerspective = p_supportPerspective;
}

void OvRendering::Data::Material::SetDrawOrder(int p_order)
{
	m_drawOrder = p_order;
}

void OvRendering::Data::Material::SetBlendable(bool p_transparent)
{
	m_blendable = p_transparent;
}

void OvRendering::Data::Material::SetUserInterface(bool p_userInterface)
{
	m_userInterface = p_userInterface;
}

void OvRendering::Data::Material::SetBackfaceCulling(bool p_backfaceCulling)
{
	m_backfaceCulling = p_backfaceCulling;
}

void OvRendering::Data::Material::SetFrontfaceCulling(bool p_frontfaceCulling)
{
	m_frontfaceCulling = p_frontfaceCulling;
}

void OvRendering::Data::Material::SetDepthTest(bool p_depthTest)
{
	m_depthTest = p_depthTest;
}

void OvRendering::Data::Material::SetDepthWriting(bool p_depthWriting)
{
	m_depthWriting = p_depthWriting;
}

void OvRendering::Data::Material::SetColorWriting(bool p_colorWriting)
{
	m_colorWriting = p_colorWriting;
}

void OvRendering::Data::Material::SetCastShadows(bool p_castShadows)
{
	m_castShadows = p_castShadows;
}

void OvRendering::Data::Material::SetReceiveShadows(bool p_receiveShadows)
{
	m_receiveShadows = p_receiveShadows;
}

void OvRendering::Data::Material::SetCapturedByReflectionProbes(bool p_capturedByReflectionProbes)
{
	m_capturedByReflectionProbes = p_capturedByReflectionProbes;
}

void OvRendering::Data::Material::SetReceiveReflections(bool p_receiveReflections)
{
	m_receiveReflections = p_receiveReflections;
}

void OvRendering::Data::Material::SetGPUInstances(int p_instances)
{
	m_gpuInstances = p_instances;
}

int OvRendering::Data::Material::GetDrawOrder() const
{
	return m_drawOrder;
}

bool OvRendering::Data::Material::IsBlendable() const
{
	return m_blendable;
}

bool OvRendering::Data::Material::IsUserInterface() const
{
	return m_userInterface;
}

bool OvRendering::Data::Material::HasBackfaceCulling() const
{
	return m_backfaceCulling;
}

bool OvRendering::Data::Material::HasFrontfaceCulling() const
{
	return m_frontfaceCulling;
}

bool OvRendering::Data::Material::HasDepthTest() const
{
	return m_depthTest;
}

bool OvRendering::Data::Material::HasDepthWriting() const
{
	return m_depthWriting;
}

bool OvRendering::Data::Material::HasColorWriting() const
{
	return m_colorWriting;
}

bool OvRendering::Data::Material::IsShadowCaster() const
{
	return m_castShadows;
}

bool OvRendering::Data::Material::IsShadowReceiver() const
{
	return m_receiveShadows;
}

bool OvRendering::Data::Material::IsCapturedByReflectionProbes() const
{
	return m_capturedByReflectionProbes;
}

bool OvRendering::Data::Material::IsReflectionReceiver() const
{
	return m_receiveReflections;
}

int OvRendering::Data::Material::GetGPUInstances() const
{
	return m_gpuInstances;
}

const OvRendering::Data::StateMask OvRendering::Data::Material::GenerateStateMask() const
{
	StateMask stateMask;
	stateMask.depthWriting = m_depthWriting;
	stateMask.colorWriting = m_colorWriting;
	stateMask.blendable = m_blendable;
	stateMask.depthTest = m_depthTest;
	stateMask.frontfaceCulling = m_frontfaceCulling;
	stateMask.backfaceCulling = m_backfaceCulling;
	return stateMask;
}

OvRendering::Data::Material::PropertyMap& OvRendering::Data::Material::GetProperties()
{
	return m_properties;
}

OvRendering::Data::FeatureSet& OvRendering::Data::Material::GetFeatures()
{
	return m_features;
}

void OvRendering::Data::Material::SetFeatures(const Data::FeatureSet& p_features)
{
	m_features = p_features;
}

void OvRendering::Data::Material::AddFeature(const std::string& p_feature)
{
	m_features.insert(p_feature);
}

void OvRendering::Data::Material::RemoveFeature(const std::string& p_feature)
{
	m_features.erase(p_feature);
}

bool OvRendering::Data::Material::HasFeature(const std::string& p_feature) const
{
	return m_features.contains(p_feature);
}

bool OvRendering::Data::Material::SupportsFeature(const std::string& p_feature) const
{
	return m_shader &&
		(m_shader->GetFeatures().contains(p_feature) ||
		 m_shader->GetEngineFeatures().contains(p_feature));
}

bool OvRendering::Data::Material::HasPass(const std::string& p_pass) const
{
	return m_shader->GetPasses().contains(p_pass);
}

bool OvRendering::Data::Material::SupportsOrthographic() const
{
	return m_supportOrthographic;
}

bool OvRendering::Data::Material::SupportsPerspective() const
{
	return m_supportPerspective;
}

bool OvRendering::Data::Material::SupportsProjectionMode(OvRendering::Settings::EProjectionMode p_projectionMode) const
{
	using enum OvRendering::Settings::EProjectionMode;

	switch (p_projectionMode)
	{
	case ORTHOGRAPHIC: return SupportsOrthographic();
	case PERSPECTIVE: return SupportsPerspective();
	}

	return true;
}

void OvRendering::Data::Material::InvalidatePropertySignature()
{
	++m_stablePropertySignatureVersion;
	++m_singleUsePropertySignatureVersion;
}

OvRendering::Data::MaterialSignatureSet OvRendering::Data::Material::CalculateSignature(
	OvRendering::HAL::ShaderProgram& p_selectedProgram,
	HAL::Texture* p_emptyTexture2D,
	HAL::Texture* p_emptyTextureCube
)
{
	ZoneScoped;

	MaterialSignatureSet signature;

	// Bind
	HashCombine(signature.bindSignature, this);
	HashCombine(signature.bindSignature, &p_selectedProgram);

	// Stable Properties
	signature.stablePropertySignature = signature.bindSignature;
	HashCombine(signature.stablePropertySignature, m_stablePropertySignatureVersion);
	HashCombine(signature.stablePropertySignature, p_emptyTexture2D);
	HashCombine(signature.stablePropertySignature, p_emptyTextureCube);

	// Single Use Properties
	signature.singleUsePropertySignature = signature.bindSignature;
	HashCombine(signature.singleUsePropertySignature, m_singleUsePropertySignatureVersion);
	HashCombine(signature.singleUsePropertySignature, p_emptyTexture2D);
	HashCombine(signature.singleUsePropertySignature, p_emptyTextureCube);

	return signature;
}

