/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <unordered_map>

#include "OvEditor/Panels/MenuBar.h"
#include "OvUI/Panels/APanel.h"
#include "OvUI/Panels/PanelWindow.h"
#include "OvUI/Modules/Canvas.h"

namespace OvEditor::Core
{
	/**
	* Handle the creation and storage of UI panels
	*/
	class PanelsManager
	{
	public:
		/**
		* Constructor
		* @param p_canvas
		*/
		PanelsManager(OvUI::Modules::Canvas& p_canvas);

		/**
		* Create a panel
		* @param p_id
		* @param p_args
		*/
		template<typename T, typename... Args>
		void CreatePanel(const std::string& p_id, Args&&... p_args)
		{
			if constexpr (std::is_base_of<OvUI::Panels::PanelWindow, T>::value)
			{
				m_panels.emplace(p_id, std::make_unique<T>(p_id, std::forward<Args>(p_args)...));
				T& instance = *static_cast<T*>(m_panels.at(p_id).get());
				GetPanelAs<OvEditor::Panels::MenuBar>("Menu Bar").RegisterPanel(instance.name, instance);
			}
			else
			{
				m_panels.emplace(p_id, std::make_unique<T>(std::forward<Args>(p_args)...));
			}

			m_canvas.AddPanel(*m_panels.at(p_id));
		}

		/**
		* Returns the panel identified by the given id casted in the given type
		* @param p_id
		*/
		template<typename T>
		T& GetPanelAs(const std::string& p_id)
		{
			return *static_cast<T*>(m_panels[p_id].get());
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<OvUI::Panels::APanel>> m_panels;
		OvUI::Modules::Canvas& m_canvas;
	};
}
