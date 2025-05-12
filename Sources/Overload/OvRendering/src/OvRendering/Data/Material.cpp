/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>
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
		int& p_textureSlot
	)
	{
		if (auto target = p_texture ? p_texture : p_fallback)
		{
			target->Bind(p_textureSlot);
			p_shader.SetUniform<int>(p_uniformName, p_textureSlot++);
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
		FillUniform();
	}
	else
	{
		m_properties.clear();
	}
}

void OvRendering::Data::Material::FillUniform()
{
	m_properties.clear();

	for (const auto& program : m_shader->GetPrograms() | std::views::values)
	{
		for (const auto& uniform : program->GetUniforms())
		{
			m_properties.emplace(uniform.name, MaterialProperty{
				.value = UniformToPropertyValue(uniform.defaultValue),
				.singleUse = false
			});
		}
	}
}

void OvRendering::Data::Material::Bind(
	OvRendering::HAL::Texture* p_emptyTexture,
	OvTools::Utils::OptRef<const Resources::Shader::FeatureSet> p_featureSetOverride
)
{
	ZoneScoped;

	using namespace OvMaths;
	using enum OvRendering::Settings::EUniformType;

	OVASSERT(IsValid(), "Attempting to bind an invalid material.");

	auto& program = m_shader->GetProgram(
		p_featureSetOverride.value_or(m_features)
	);

	program.Bind();

	int textureSlot = 0;

	for (auto& [name, prop] : m_properties)
	{
		const auto uniformData = program.GetUniformInfo(name);

		// Skip this property if the current program isn't using its associated uniform
		if (!uniformData)
		{
			continue;
		}

		auto& value = prop.value;
		auto uniformType = uniformData->type;

		// Visitor to handle each variant type
		auto visitor = [&](auto&& arg) {
			using PropertyType = std::decay_t<decltype(arg)>;

			if constexpr (std::same_as<PropertyType, bool>)
			{
				if (uniformType == BOOL)
				{
					program.SetUniform<int>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, int>)
			{
				if (uniformType == INT)
				{
					program.SetUniform<int>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, float>)
			{
				if (uniformType == FLOAT)
				{
					program.SetUniform<float>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, FVector2>)
			{
				if (uniformType == FLOAT_VEC2)
				{
					program.SetUniform<FVector2>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, FVector3>)
			{
				if (uniformType == FLOAT_VEC3)
				{
					program.SetUniform<FVector3>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, FVector4>)
			{
				if (uniformType == FLOAT_VEC4)
				{
					program.SetUniform<FVector4>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, FMatrix4>)
			{
				if (uniformType == FLOAT_MAT4)
				{
					program.SetUniform<FMatrix4>(name, arg);
				}
			}
			else if constexpr (std::same_as<PropertyType, HAL::TextureHandle*>)
			{
				if (uniformType == SAMPLER_2D)
				{
					BindTexture(program, name, arg, p_emptyTexture, textureSlot);
				}
			}
			else if constexpr (std::same_as<PropertyType, Resources::Texture*>)
			{
				if (uniformType == SAMPLER_2D)
				{
					BindTexture(program, name, arg ? &arg->GetTexture() : nullptr, p_emptyTexture, textureSlot);
				}
			}
		};

		std::visit(visitor, value);

		if (prop.singleUse)
		{
			value = UniformToPropertyValue(uniformData->defaultValue);
		}
	}
}

void OvRendering::Data::Material::Unbind() const
{
	OVASSERT(IsValid(), "Attempting to unbind an invalid material.");
	m_shader->GetProgram().Unbind();
}

void OvRendering::Data::Material::SetProperty(const std::string p_name, const MaterialPropertyType& p_value, bool p_singleUse)
{
	OVASSERT(IsValid(), "Attempting to SetProperty on an invalid material.");

	if (m_properties.find(p_name) != m_properties.end())
	{
		const auto property = MaterialProperty{
			p_value,
			p_singleUse
		};

		m_properties[p_name] = property;
	}
	else
	{
		OVLOG_ERROR("Material Set failed: Uniform not found");
	}
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

void OvRendering::Data::Material::SetGPUInstances(int p_instances)
{
	m_gpuInstances = p_instances;
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
	stateMask.userInterface = m_userInterface;
	stateMask.frontfaceCulling = m_frontfaceCulling;
	stateMask.backfaceCulling = m_backfaceCulling;
	return stateMask;
}

OvRendering::Data::Material::PropertyMap& OvRendering::Data::Material::GetProperties()
{
	return m_properties;
}

OvRendering::Resources::Shader::FeatureSet& OvRendering::Data::Material::GetFeatures()
{
	return m_features;
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
	return m_shader->GetFeatures().contains(p_feature);
}
