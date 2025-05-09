/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <json.hpp>

#include <OvCore/Resources/Loaders/MaterialLoader.h>

#include <OvDebug/Logger.h>
#include <json_fwd.hpp>
#include <iostream>
#include <fstream>

OvCore::Resources::Material * OvCore::Resources::Loaders::MaterialLoader::Create(const std::string & p_path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(p_path.c_str());
	if (!doc.Error())
	{
		tinyxml2::XMLNode* root = doc.FirstChild();

		Material* material = new Material();

		material->OnDeserialize(doc, root);

		return material;
	}
	else
	{
		return nullptr;
	}
}

void OvCore::Resources::Loaders::MaterialLoader::Reload(Material& p_material, const std::string& p_path)
{
	if (std::filesystem::exists(std::format("{}.json", p_path)))
	{
		std::ifstream file(std::format("{}.json", p_path));
		nlohmann::json j;
		file >> j;
		p_material = j.template get<std::remove_reference<decltype(p_material)>::type>();
		OVLOG_INFO("[MATERIAL] \"" + p_path + "\" Reloaded");
		return;
	}

	tinyxml2::XMLDocument doc;
	doc.LoadFile(p_path.c_str());
	if (!doc.Error())
	{
		tinyxml2::XMLNode* root = doc.FirstChild();
		p_material.OnDeserialize(doc, root);
		OVLOG_INFO("[MATERIAL] \"" + p_path + "\" Reloaded");
	}
}

void OvCore::Resources::Loaders::MaterialLoader::Save(Material& p_material, const std::string& p_path)
{
	nlohmann::json j = p_material;
	std::cout << j << std::endl;
	std::ofstream file(std::format("{}.json", p_path));
	file << std::setw(4) << j << std::endl;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* node = doc.NewElement("root");
	doc.InsertFirstChild(node);

	p_material.OnSerialize(doc, node);

	if (doc.SaveFile(p_path.c_str()) == tinyxml2::XML_SUCCESS)
		OVLOG_INFO("[MATERIAL] \"" + p_path + "\": Saved");
	else
		OVLOG_ERROR("[MATERIAL] \"" + p_path + "\": Failed to save");
}

bool OvCore::Resources::Loaders::MaterialLoader::Destroy(Material *& p_material)
{
	if (p_material)
	{
		delete p_material;
		p_material = nullptr;

		return true;
	}

	return false;
}