/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

namespace OvCore::ResourceManagement
{
	/**
	* Stores host-provided default resources used by UI components.
	*/
	class UIResourceRegistry
	{
	public:
		struct Definition
		{
			std::string imageMaterialPath;
			std::string textMaterialPath;
			std::string defaultFontPath;
		};

		/**
		* Provides the UI resource definition used by UI components.
		* @param p_definition
		*/
		void ProvideDefinition(Definition p_definition);

		/**
		* Returns the current UI resource definition.
		*/
		const Definition& GetDefinition() const;

	private:
		Definition m_definition;
	};
}
