/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <sstream>

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

		Serializer::SerializeString(p_doc, uniform, "name", name);

		auto visitor = [&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			using enum EUniformType;

			if constexpr (std::same_as<T, bool>)
			{
				Serializer::SerializeInt(p_doc, uniform, "value", arg);
			}
			else if constexpr (std::same_as<T, int>)
			{
				Serializer::SerializeInt(p_doc, uniform, "value", arg);
			}
			else if constexpr (std::same_as<T, float>)
			{
				Serializer::SerializeFloat(p_doc, uniform, "value", arg);
			}
			else if constexpr (std::same_as<T, FVector2>)
			{
				Serializer::SerializeVec2(p_doc, uniform, "value", arg);
			}
			else if constexpr (std::same_as<T, FVector3>)
			{
				Serializer::SerializeVec3(p_doc, uniform, "value", arg);
			}
			else if constexpr (std::same_as<T, FVector4>)
			{
				Serializer::SerializeVec4(p_doc, uniform, "value", arg);
			}
			else if constexpr (std::same_as<T, OvRendering::Resources::Texture*>)
			{
				Serializer::SerializeTexture(p_doc, uniform, "value", arg);
			}
			// No need to handle TextureHandle* here as it's not serializable (only texture assets are)
		};

		std::visit(visitor, value);
	}

	std::string features;

	for (auto it = m_features.begin(); it != m_features.end(); ++it)
	{
		features += *it;

		if (std::next(it) != m_features.end())
		{
			features += ",";
		}
	}

	Serializer::SerializeString(p_doc, p_node, "features", features);
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

		const auto uniformsNode = p_node->FirstChildElement("uniforms"); // Access to "Uniforms" (Every uniform will be attached to "Uniforms")		

		if (uniformsNode)
		{
			// Iterate over every <uniform>...</uniform>
			for (auto uniform = uniformsNode->FirstChildElement("uniform"); uniform; uniform = uniform->NextSiblingElement("uniform"))
			{
				if (const auto uniformNameElement = uniform->FirstChildElement("name"))
				{
					const auto propName = uniformNameElement->GetText();

					// Make sure that there is a property with the same name.
					// The type of the property value will be used to determine which type to deserialize.
					// This means that the serialized data doesn't hold the type information, and if the
					// type changes after serialization, it will not be deserialized correctly (skipped).
					if (const auto prop = GetProperty(propName))
					{
						auto visitor = [&](auto&& arg) {
							using T = std::decay_t<decltype(arg)>;
							using namespace OvMaths;

							if constexpr (std::same_as<T, bool>)
							{
								SetProperty(propName, Serializer::DeserializeBoolean(p_doc, uniform, "value"));
							}
							else if constexpr (std::same_as<T, int>)
							{
								SetProperty(propName, Serializer::DeserializeInt(p_doc, uniform, "value"));
							}
							else if constexpr (std::same_as<T, float>)
							{
								SetProperty(propName, Serializer::DeserializeFloat(p_doc, uniform, "value"));
							}
							else if constexpr (std::same_as<T, FVector2>)
							{
								SetProperty(propName, Serializer::DeserializeVec2(p_doc, uniform, "value"));
							}
							else if constexpr (std::same_as<T, FVector3>)
							{
								SetProperty(propName, Serializer::DeserializeVec3(p_doc, uniform, "value"));
							}
							else if constexpr (std::same_as<T, FVector4>)
							{
								SetProperty(propName, Serializer::DeserializeVec4(p_doc, uniform, "value"));
							}
							else if constexpr (std::same_as<T, OvRendering::Resources::Texture*>)
							{
								SetProperty(propName, Serializer::DeserializeTexture(p_doc, uniform, "value"));
							}
							// No need to handle TextureHandle* here as it's not serializable (only texture assets are)
						};

						std::visit(visitor, prop->value);
					}
				}
			}
		}
	}

	const auto features = Serializer::DeserializeString(p_doc, p_node, "features");

	// Parse features (comma-separated)
	std::string feature;
	std::istringstream featureStream(features);
	while (std::getline(featureStream, feature, ','))
	{
		if (!feature.empty())
		{
			AddFeature(feature);
		}
	}
}
