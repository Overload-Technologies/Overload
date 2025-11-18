/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <any>
#include <map>

#include <OvRendering/Data/Material.h>

#include "OvCore/API/ISerializable.h"

namespace OvCore::Resources
{
	/**
	* Engine material, which inherits from OvRendering Material and adds serialization capabilities
	*/
	class Material : public OvRendering::Data::Material, public API::ISerializable
	{
	public:
		/**
		* Serialize the material
		* @param p_doc
		* @param p_node
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Deserialize the material
		* @param p_doc
		* @param p_node
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		const std::string path;
	};
}
