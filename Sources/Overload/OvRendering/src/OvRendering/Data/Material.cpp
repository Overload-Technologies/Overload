/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Data/Material.h"
#include "OvRendering/HAL/UniformBuffer.h"
#include "OvRendering/HAL/TextureHandle.h"
#include "OvRendering/Resources/Texture.h"
#include <OvTools/Utils/OptRef.h>

//TODO: Add constructor with a shader reference

OvRendering::Data::Material::Material(OvRendering::Resources::Shader* p_shader)
{
	SetShader(p_shader);
}

void OvRendering::Data::Material::SetShader(OvRendering::Resources::Shader* p_shader)
{
	m_shader = p_shader;

	if (m_shader)
	{
		// TODO: Move that line to Engine Material
		// OvRendering::HAL::UniformBuffer::BindBlockToShader(*m_shader, "EngineUBO");
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

	for (const auto& uniform : m_shader->GetProgram().GetUniforms())
	{
		m_properties.emplace(uniform.name, MaterialProperty{ uniform.defaultValue, false });
	}
}

void OvRendering::Data::Material::Bind(OvRendering::Resources::Texture* p_emptyTexture)
{
	if (HasShader())
	{
		using namespace OvMaths;
		using namespace OvRendering::Resources;

		m_shader->GetProgram().Bind();

		int textureSlot = 0;

		for (auto& [name, prop] : m_properties)
		{
			auto& value = prop.value;

			const auto uniformData = m_shader->GetProgram().GetUniformInfo(name);

			if (uniformData)
			{
				switch (uniformData->type)
				{
				case OvRendering::Settings::EUniformType::BOOL:			if (value.type() == typeid(bool))		m_shader->GetProgram().SetUniform<int>(name, std::any_cast<bool>(value));			break;
				case OvRendering::Settings::EUniformType::INT:			if (value.type() == typeid(int))		m_shader->GetProgram().SetUniform<int>(name, std::any_cast<int>(value));			break;
				case OvRendering::Settings::EUniformType::FLOAT:		if (value.type() == typeid(float))		m_shader->GetProgram().SetUniform<float>(name, std::any_cast<float>(value));		break;
				case OvRendering::Settings::EUniformType::FLOAT_VEC2:	if (value.type() == typeid(FVector2))	m_shader->GetProgram().SetUniform<FVector2>(name, std::any_cast<FVector2>(value));		break;
				case OvRendering::Settings::EUniformType::FLOAT_VEC3:	if (value.type() == typeid(FVector3))	m_shader->GetProgram().SetUniform<FVector3>(name, std::any_cast<FVector3>(value));		break;
				case OvRendering::Settings::EUniformType::FLOAT_VEC4:	if (value.type() == typeid(FVector4))	m_shader->GetProgram().SetUniform<FVector4>(name, std::any_cast<FVector4>(value));		break;
				case OvRendering::Settings::EUniformType::FLOAT_MAT4:	if (value.type() == typeid(FMatrix4))	m_shader->GetProgram().SetUniform<FMatrix4>(name, std::any_cast<FMatrix4>(value));		break;
				case OvRendering::Settings::EUniformType::SAMPLER_2D:
				{
					if (value.type() == typeid(HAL::TextureHandle))
					{
						auto tex = std::any_cast<HAL::TextureHandle>(value);
						tex.Bind(textureSlot);
						m_shader->GetProgram().SetUniform<int>(uniformData->name, textureSlot++);
					}
					else if (value.type() == typeid(HAL::Texture))
					{
						auto tex = std::any_cast<HAL::Texture>(value);
						tex.Bind(textureSlot);
						m_shader->GetProgram().SetUniform<int>(uniformData->name, textureSlot++);
					}
					else if (value.type() == typeid(Resources::Texture*))
					{
						if (auto tex = std::any_cast<Resources::Texture*>(value); tex)
						{
							tex->GetTexture().Bind(textureSlot);
							m_shader->GetProgram().SetUniform<int>(uniformData->name, textureSlot++);
						}
						else if (p_emptyTexture)
						{
							p_emptyTexture->GetTexture().Bind(textureSlot);
							m_shader->GetProgram().SetUniform<int>(uniformData->name, textureSlot++);
						}
					}
					else if (value.type() == typeid(OvTools::Utils::OptRef<HAL::Texture>))
					{
						if (const auto tex = std::any_cast<OvTools::Utils::OptRef<HAL::Texture>>(value); tex)
						{
							tex->Bind(textureSlot);
							m_shader->GetProgram().SetUniform<int>(uniformData->name, textureSlot++);
						}
					}
				}
				}

				if (prop.singleUse)
				{
					value = uniformData->defaultValue;
				}
			}
		}
	}
}

void OvRendering::Data::Material::UnBind() const
{
	if (HasShader())
	{
		m_shader->GetProgram().Unbind();
	}
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
