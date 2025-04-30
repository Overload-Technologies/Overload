/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tinyxml2.h>

#include <OvCore/Resources/Material.h>

void OvCore::Resources::Material::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;
	using namespace OvRendering::Settings;
	using namespace OvMaths;

	Serializer::SerializeShader(p_doc, p_node, "shader", m_shader);

	tinyxml2::XMLNode* settingsNode = p_doc.NewElement("settings");
	p_node->InsertEndChild(settingsNode);

	Serializer::SerializeBoolean(p_doc, settingsNode, "blendable", m_blendable);
	Serializer::SerializeBoolean(p_doc, settingsNode, "backface_culling", m_backfaceCulling);
	Serializer::SerializeBoolean(p_doc, settingsNode, "frontface_culling", m_frontfaceCulling);
	Serializer::SerializeBoolean(p_doc, settingsNode, "depth_test", m_depthTest);
	Serializer::SerializeBoolean(p_doc, settingsNode, "depth_writing", m_depthWriting);
	Serializer::SerializeBoolean(p_doc, settingsNode, "color_writing", m_colorWriting);
	Serializer::SerializeBoolean(p_doc, settingsNode, "cast_shadows", m_castShadows);
	Serializer::SerializeBoolean(p_doc, settingsNode, "receive_shadows", m_receiveShadows);
	Serializer::SerializeBoolean(p_doc, settingsNode, "user_interface", m_userInterface);
	Serializer::SerializeInt(p_doc, settingsNode, "gpu_instances", m_gpuInstances);

	// Create "Uniforms" (Every uniform will be attached to "Uniforms")
	tinyxml2::XMLNode* uniformsNode = p_doc.NewElement("uniforms");
	p_node->InsertEndChild(uniformsNode);

	for (const auto& [name, prop] : m_properties)
	{
		auto& value = prop.value;
		tinyxml2::XMLNode* uniform = p_doc.NewElement("uniform");
		uniformsNode->InsertEndChild(uniform);

		const auto uniformInfo = m_shader->GetProgram().GetUniformInfo(name);
		Serializer::SerializeString(p_doc, uniform, "name", name);

		if (uniformInfo && !std::holds_alternative<std::monostate>(value))
		{
			auto visitor = [&](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				using enum EUniformType;
				const auto type = uniformInfo->type;

				if constexpr (std::same_as<T, bool>)
				{
					if (type == BOOL)
					{
						Serializer::SerializeInt(p_doc, uniform, "value", arg);
					}
				}
				else if constexpr (std::same_as<T, int>)
				{
					if (type == INT)
					{
						Serializer::SerializeInt(p_doc, uniform, "value", arg);
					}
				}
				else if constexpr (std::same_as<T, float>)
				{
					if (type == FLOAT)
					{
						Serializer::SerializeFloat(p_doc, uniform, "value", arg);
					}
				}
				else if constexpr (std::same_as<T, FVector2>)
				{
					if (type == FLOAT_VEC2)
					{
						Serializer::SerializeVec2(p_doc, uniform, "value", arg);
					}
				}
				else if constexpr (std::same_as<T, FVector3>)
				{
					if (type == FLOAT_VEC3)
					{
						Serializer::SerializeVec3(p_doc, uniform, "value", arg);
					}
				}
				else if constexpr (std::same_as<T, FVector4>)
				{
					if (type == FLOAT_VEC4)
					{
						Serializer::SerializeVec4(p_doc, uniform, "value", arg);
					}
				}
				else if constexpr (std::same_as<T, OvRendering::Resources::Texture*>)
				{
					if (type == SAMPLER_2D)
					{
						Serializer::SerializeTexture(p_doc, uniform, "value", arg);
					}
				}
				// No need to handle TextureHandle* here as it's not serializable (only texture assets are)
			};

			std::visit(visitor, value);
		}
	}
}

void OvCore::Resources::Material::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;

	tinyxml2::XMLNode* settingsNode = p_node->FirstChildElement("settings");
	
	if (settingsNode)
	{
		Serializer::DeserializeBoolean(p_doc, settingsNode, "blendable", m_blendable);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "backface_culling", m_backfaceCulling);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "frontface_culling", m_frontfaceCulling);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "depth_test", m_depthTest);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "depth_writing", m_depthWriting);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "color_writing", m_colorWriting);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "cast_shadows", m_castShadows);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "receive_shadows", m_receiveShadows);
		Serializer::DeserializeBoolean(p_doc, settingsNode, "user_interface", m_userInterface);
		Serializer::DeserializeInt(p_doc, settingsNode, "gpu_instances", m_gpuInstances);
	}

	/* We get the shader with Deserialize method */
	const auto shader = Serializer::DeserializeShader(p_doc, p_node, "shader");

	/* We verify that the shader is valid (Not null) */
	if (shader)
	{
		/* If the shader is valid, we set it to the material (Modify m_shader pointer + Query + FillUniforms) */
		SetShader(shader);

		tinyxml2::XMLNode* uniformsNode = p_node->FirstChildElement("uniforms"); // Access to "Uniforms" (Every uniform will be attached to "Uniforms")		

		if (uniformsNode)
		{
			/* We iterate over every <uniform>...</uniform> */
			for (auto uniform = uniformsNode->FirstChildElement("uniform"); uniform; uniform = uniform->NextSiblingElement("uniform"))
			{
				/* Verify that the uniform node contains a "name" element */
				if (auto uniformNameElement = uniform->FirstChildElement("name"); uniformNameElement)
				{
					const std::string uniformName = uniformNameElement->GetText();

					/* We collect information about the uniform (The uniform is identified in the shader by its name) */
					const auto uniformInfo = m_shader->GetProgram().GetUniformInfo(uniformName);

					/* We verify that the uniform is existant is the current shader */
					if (uniformInfo)
					{
						/* Deserialization of the uniform value depending on the uniform type (Deserialization result to std::any) */
						switch (uniformInfo->type)
						{
							using enum OvRendering::Settings::EUniformType;
							case BOOL: SetProperty(uniformInfo->name, Serializer::DeserializeBoolean(p_doc, uniform, "value")); break;
							case INT: SetProperty(uniformInfo->name, Serializer::DeserializeInt(p_doc, uniform, "value")); break;
							case FLOAT: SetProperty(uniformInfo->name, Serializer::DeserializeFloat(p_doc, uniform, "value")); break;
							case FLOAT_VEC2: SetProperty(uniformInfo->name, Serializer::DeserializeVec2(p_doc, uniform, "value")); break;
							case FLOAT_VEC3: SetProperty(uniformInfo->name, Serializer::DeserializeVec3(p_doc, uniform, "value")); break;
							case FLOAT_VEC4: SetProperty(uniformInfo->name, Serializer::DeserializeVec4(p_doc, uniform, "value")); break;
							case FLOAT_MAT4: SetProperty(uniformInfo->name, Serializer::DeserializeMat4(p_doc, uniform, "value")); break;
							case SAMPLER_2D: SetProperty(uniformInfo->name, Serializer::DeserializeTexture(p_doc, uniform, "value")); break;
						}
					}
				}
			}
		}
	}
}
