/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvCore/Helpers/GUIDrawer.h>
#include <OvDebug/Assertion.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvRendering/Settings/ETextureFilteringMode.h>
#include <OvTools/Utils/PathParser.h>

namespace
{
	template<OvRendering::Settings::ETextureFilteringMode FilteringMode>
	auto CreateTexture(const std::filesystem::path& p_path)
	{
		return OvRendering::Resources::Loaders::TextureLoader::Create(
			p_path.string(),
			FilteringMode,
			FilteringMode,
			false
		);
	}

	auto CreateModel(const std::filesystem::path& p_path)
	{
		const auto modelParserFlags =
			OvRendering::Resources::Parsers::EModelParserFlags::TRIANGULATE |
			OvRendering::Resources::Parsers::EModelParserFlags::GEN_SMOOTH_NORMALS |
			OvRendering::Resources::Parsers::EModelParserFlags::OPTIMIZE_MESHES |
			OvRendering::Resources::Parsers::EModelParserFlags::OPTIMIZE_GRAPH |
			OvRendering::Resources::Parsers::EModelParserFlags::FIND_INSTANCES |
			OvRendering::Resources::Parsers::EModelParserFlags::CALC_TANGENT_SPACE |
			OvRendering::Resources::Parsers::EModelParserFlags::JOIN_IDENTICAL_VERTICES |
			OvRendering::Resources::Parsers::EModelParserFlags::DEBONE |
			OvRendering::Resources::Parsers::EModelParserFlags::FIND_INVALID_DATA |
			OvRendering::Resources::Parsers::EModelParserFlags::IMPROVE_CACHE_LOCALITY |
			OvRendering::Resources::Parsers::EModelParserFlags::GEN_UV_COORDS |
			OvRendering::Resources::Parsers::EModelParserFlags::PRE_TRANSFORM_VERTICES |
			OvRendering::Resources::Parsers::EModelParserFlags::GLOBAL_SCALE;

		return OvRendering::Resources::Loaders::ModelLoader::Create(
			p_path.string(),
			modelParserFlags
		);
	}

	auto CreateShader(const std::filesystem::path& p_path)
	{
		return OvRendering::Resources::Loaders::ShaderLoader::Create(
			p_path.string()
		);
	}

	template<typename T>
	auto ValidateResources(const std::unordered_map<std::string, T>& p_resources)
	{
		for (const auto& [id, resource] : p_resources)
		{
			OVASSERT(resource != nullptr, "Failed to load resource with ID: " + id);
		}
	}

	template<typename T>
	T TryGetResource(std::unordered_map<std::string, T>& p_resources, const std::string& p_id)
	{
		return
			p_resources.find(p_id) != p_resources.end() ?
			p_resources.at(p_id) :
			nullptr;
	}
}

OvEditor::Core::EditorResources::EditorResources(const std::string& p_editorAssetsPath)
{
	using namespace OvRendering::Resources::Loaders;
	using enum OvRendering::Settings::ETextureFilteringMode;

	const auto editorAssetsPath = std::filesystem::path{ p_editorAssetsPath };
	const auto texturesFolder = editorAssetsPath / "Textures";
	const auto modelsFolder = editorAssetsPath / "Models";
	const auto shadersFolder = editorAssetsPath / "Shaders";

	m_textures = {
		{"Play", CreateTexture<LINEAR>(texturesFolder / "Play.png")},
		{"Pause", CreateTexture<LINEAR>(texturesFolder / "Pause.png")},
		{"Stop", CreateTexture<LINEAR>(texturesFolder / "Stop.png")},
		{"Next", CreateTexture<LINEAR>(texturesFolder / "Next.png")},
		{"Refresh", CreateTexture<LINEAR>(texturesFolder / "Refresh.png")},
		{"File", CreateTexture<LINEAR>(texturesFolder / "File.png")},
		{"Folder", CreateTexture<LINEAR>(texturesFolder / "Folder.png")},
		{"Texture", CreateTexture<LINEAR>(texturesFolder / "Texture.png")},
		{"Model", CreateTexture<LINEAR>(texturesFolder / "Model.png")},
		{"Shader", CreateTexture<LINEAR>(texturesFolder / "Shader.png")},
		{"Shader_Part", CreateTexture<LINEAR>(texturesFolder / "Shader_Part.png")},
		{"Material", CreateTexture<LINEAR>(texturesFolder / "Material.png")},
		{"Scene", CreateTexture<LINEAR>(texturesFolder / "Scene.png")},
		{"Sound", CreateTexture<LINEAR>(texturesFolder / "Sound.png")},
		{"Script", CreateTexture<LINEAR>(texturesFolder / "Script.png")},
		{"Font", CreateTexture<LINEAR>(texturesFolder / "Font.png")},
		{"Point_Light", CreateTexture<NEAREST>(texturesFolder / "Point_Light.png")},
		{"Spot_Light", CreateTexture<NEAREST>(texturesFolder / "Spot_Light.png")},
		{"Directional_Light", CreateTexture<NEAREST>(texturesFolder / "Directional_Light.png")},
		{"Ambient_Box_Light", CreateTexture<NEAREST>(texturesFolder / "Ambient_Box_Light.png")},
		{"Ambient_Sphere_Light", CreateTexture<NEAREST>(texturesFolder / "Ambient_Sphere_Light.png")},
		{"Empty_Texture", CreateTexture<LINEAR>(texturesFolder / "Empty_Texture.png")}
	};

	m_models = {
		{"Cube", CreateModel(modelsFolder / "Cube.fbx")},
		{"Cylinder", CreateModel(modelsFolder / "Cylinder.fbx")},
		{"Plane", CreateModel(modelsFolder / "Plane.fbx")},
		{"Vertical_Plane", CreateModel(modelsFolder / "Vertical_Plane.fbx")},
		{"Roll", CreateModel(modelsFolder / "Roll.fbx")},
		{"Sphere", CreateModel(modelsFolder / "Sphere.fbx")},
		{"Arrow_Translate", CreateModel(modelsFolder / "Arrow_Translate.fbx")},
		{"Arrow_Rotate", CreateModel(modelsFolder / "Arrow_Rotate.fbx")},
		{"Arrow_Scale", CreateModel(modelsFolder / "Arrow_Scale.fbx")},
		{"Arrow_Picking", CreateModel(modelsFolder / "Arrow_Picking.fbx")},
		{"Camera", CreateModel(modelsFolder / "Camera.fbx")}
	};

	m_shaders = {
		{"Grid", CreateShader(shadersFolder / "Grid.ovfx")},
		{"Gizmo", CreateShader(shadersFolder / "Gizmo.ovfx")},
		{"Billboard", CreateShader(shadersFolder / "Billboard.ovfx")},
	};

	// Ensure that all resources have been loaded successfully
	ValidateResources(m_textures);
	ValidateResources(m_models);
	ValidateResources(m_shaders);

	// Register the empty texture for the GUIDrawer to use it when a texture is missing
	OvCore::Helpers::GUIDrawer::ProvideEmptyTexture(*m_textures["Empty_Texture"]);
}

OvEditor::Core::EditorResources::~EditorResources()
{
	for (auto& [_, texture] : m_textures)
	{
		OvRendering::Resources::Loaders::TextureLoader::Destroy(texture);
	}

	for (auto& [_, mesh] : m_models)
	{
		OvRendering::Resources::Loaders::ModelLoader::Destroy(mesh);
	}

	for (auto& [_, shader] : m_shaders)
	{
		OvRendering::Resources::Loaders::ShaderLoader::Destroy(shader);
	}
}

OvRendering::Resources::Texture* OvEditor::Core::EditorResources::GetFileIcon(const std::string& p_filename)
{
	using namespace OvTools::Utils;

	const PathParser::EFileType fileType = PathParser::GetFileType(p_filename);

	return GetTexture(
		fileType == PathParser::EFileType::UNKNOWN ?
		"File" : // If the file type is unknown, we return the "File" icon
		PathParser::FileTypeToString(fileType) // Otherwise we return the icon corresponding to the file type
	);
}

OvRendering::Resources::Texture* OvEditor::Core::EditorResources::GetTexture(const std::string& p_id)
{
	return TryGetResource(m_textures, p_id);
}

OvRendering::Resources::Model* OvEditor::Core::EditorResources::GetModel(const std::string& p_id)
{
	return TryGetResource(m_models, p_id);
}

OvRendering::Resources::Shader* OvEditor::Core::EditorResources::GetShader(const std::string& p_id)
{
	return TryGetResource(m_shaders, p_id);
}
