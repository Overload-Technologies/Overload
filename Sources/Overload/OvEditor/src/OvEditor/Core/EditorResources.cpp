/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Helpers/GUIDrawer.h>

#include <OvRendering/Settings/ETextureFilteringMode.h>

#include <OvTools/Utils/PathParser.h>

#include "OvEditor/Core/EditorResources.h"
#include "OvEditor/Resources/RawTextures.h"

OvEditor::Core::EditorResources::EditorResources(const std::string& p_editorAssetsPath)
{
	using namespace OvRendering::Resources::Loaders;

	std::string iconsFolder		= p_editorAssetsPath + "Icons\\";
	std::string modelsFolder	= p_editorAssetsPath + "Models\\";
	std::string shadersFolder	= p_editorAssetsPath + "Shaders\\";

	OvRendering::Resources::Parsers::EModelParserFlags modelParserFlags = OvRendering::Resources::Parsers::EModelParserFlags::NONE;

	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::TRIANGULATE;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GEN_SMOOTH_NORMALS;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::OPTIMIZE_MESHES;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::OPTIMIZE_GRAPH;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FIND_INSTANCES;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::CALC_TANGENT_SPACE;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::JOIN_IDENTICAL_VERTICES;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::DEBONE;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FIND_INVALID_DATA;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::IMPROVE_CACHE_LOCALITY;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GEN_UV_COORDS;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::PRE_TRANSFORM_VERTICES;
	modelParserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GLOBAL_SCALE;

	OvRendering::Settings::ETextureFilteringMode minFilterEditor = OvRendering::Settings::ETextureFilteringMode::LINEAR;
	OvRendering::Settings::ETextureFilteringMode magFilterEditor = OvRendering::Settings::ETextureFilteringMode::LINEAR;

	OvRendering::Settings::ETextureFilteringMode minFilterBillboard = OvRendering::Settings::ETextureFilteringMode::NEAREST;
	OvRendering::Settings::ETextureFilteringMode magFilterBillboard = OvRendering::Settings::ETextureFilteringMode::NEAREST;

	/* Buttons */

	{
		std::vector<uint64_t> raw = BUTTON_PLAY;
		m_textures["Button_Play"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 64, 64, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = BUTTON_PAUSE;
		m_textures["Button_Pause"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 64, 64, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = BUTTON_STOP;
		m_textures["Button_Stop"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 64, 64, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = BUTTON_NEXT;
		m_textures["Button_Next"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 64, 64, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = BUTTON_REFRESH;
		m_textures["Button_Refresh"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 64, 64, minFilterEditor, magFilterEditor, false);
	}

	/* Icons */
	{
		std::vector<uint64_t> raw = ICON_FILE;
		m_textures["Icon_Unknown"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_FOLDER;
		m_textures["Icon_Folder"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_TEXTURE;
		m_textures["Icon_Texture"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_MODEL;
		m_textures["Icon_Model"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_SHADER;
		m_textures["Icon_Shader"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_MATERIAL;
		m_textures["Icon_Material"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_SCENE;
		m_textures["Icon_Scene"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_SOUND;
		m_textures["Icon_Sound"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_SCRIPT;
		m_textures["Icon_Script"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = ICON_FONT;
		m_textures["Icon_Font"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 16, 16, minFilterEditor, magFilterEditor, false);
	}

	{
		std::vector<uint64_t> raw = BILL_PLIGHT;
		m_textures["Bill_Point_Light"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 128, 128, minFilterBillboard, magFilterBillboard, false);
	}

	{
		std::vector<uint64_t> raw = BILL_SLIGHT;
		m_textures["Bill_Spot_Light"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 128, 128, minFilterBillboard, magFilterBillboard, false);
	}

	{
		std::vector<uint64_t> raw = BILL_DLIGHT;
		m_textures["Bill_Directional_Light"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 128, 128, minFilterBillboard, magFilterBillboard, false);
	}

	{
		std::vector<uint64_t> raw = BILL_ABLIGHT;
		m_textures["Bill_Ambient_Box_Light"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 128, 128, minFilterBillboard, magFilterBillboard, false);
	}

	{
		std::vector<uint64_t> raw = BILL_ASLIGHT;
		m_textures["Bill_Ambient_Sphere_Light"] = TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 128, 128, minFilterBillboard, magFilterBillboard, false);
	}

	m_textures["Icon_Shader_Part"] = TextureLoader::Create(iconsFolder + "puzzle.png", minFilterEditor, magFilterEditor, false);

	/* Models */
	m_models["Cube"]			= ModelLoader::Create(modelsFolder + "Cube.fbx", modelParserFlags);
	m_models["Cylinder"]		= ModelLoader::Create(modelsFolder + "Cylinder.fbx", modelParserFlags);
	m_models["Plane"]			= ModelLoader::Create(modelsFolder + "Plane.fbx", modelParserFlags);
	m_models["Vertical_Plane"]	= ModelLoader::Create(modelsFolder + "Vertical_Plane.fbx", modelParserFlags);
	m_models["Roll"]			= ModelLoader::Create(modelsFolder + "Roll.fbx", modelParserFlags);
	m_models["Sphere"]			= ModelLoader::Create(modelsFolder + "Sphere.fbx", modelParserFlags);
	m_models["Arrow_Translate"]	= ModelLoader::Create(modelsFolder + "Arrow_Translate.fbx", modelParserFlags);
	m_models["Arrow_Rotate"]	= ModelLoader::Create(modelsFolder + "Arrow_Rotate.fbx", modelParserFlags);
	m_models["Arrow_Scale"]		= ModelLoader::Create(modelsFolder + "Arrow_Scale.fbx", modelParserFlags);
	m_models["Arrow_Picking"]	= ModelLoader::Create(modelsFolder + "Arrow_Picking.fbx", modelParserFlags);
	m_models["Camera"]			= ModelLoader::Create(modelsFolder + "Camera.fbx", modelParserFlags);

	/* Shaders */
	m_shaders["Grid"] = ShaderLoader::Create(shadersFolder + "Grid.ovfx");
	m_shaders["Gizmo"] = ShaderLoader::Create(shadersFolder + "Gizmo.ovfx");
	m_shaders["Billboard"] = ShaderLoader::Create(shadersFolder + "Billboard.ovfx");

	/* From memory */
	{
		std::vector<uint64_t> raw = EMPTY_TEXTURE;
		m_textures["Empty_Texture"] = OvRendering::Resources::Loaders::TextureLoader::CreateFromMemory(reinterpret_cast<uint8_t*>(raw.data()), 64, 64, minFilterEditor, magFilterEditor, false);
		OvCore::Helpers::GUIDrawer::ProvideEmptyTexture(*m_textures["Empty_Texture"]);
	}
}

OvEditor::Core::EditorResources::~EditorResources()
{
	for (auto[id, texture] : m_textures)
		OvRendering::Resources::Loaders::TextureLoader::Destroy(texture);

	for (auto [id, mesh] : m_models)
		OvRendering::Resources::Loaders::ModelLoader::Destroy(mesh);

	for (auto [id, shader] : m_shaders)
		OvRendering::Resources::Loaders::ShaderLoader::Destroy(shader);
}

OvRendering::Resources::Texture* OvEditor::Core::EditorResources::GetFileIcon(const std::string& p_filename)
{
	using namespace OvTools::Utils;
	return GetTexture("Icon_" + PathParser::FileTypeToString(PathParser::GetFileType(p_filename)));
}

OvRendering::Resources::Texture* OvEditor::Core::EditorResources::GetTexture(const std::string& p_id)
{
	if (m_textures.find(p_id) != m_textures.end())
		return m_textures.at(p_id);

	return nullptr;
}

OvRendering::Resources::Model* OvEditor::Core::EditorResources::GetModel(const std::string& p_id)
{
	if (m_models.find(p_id) != m_models.end())
		return m_models.at(p_id);

	return nullptr;
}

OvRendering::Resources::Shader* OvEditor::Core::EditorResources::GetShader(const std::string& p_id)
{
	if (m_shaders.find(p_id) != m_shaders.end())
		return m_shaders.at(p_id);

	return nullptr;
}
