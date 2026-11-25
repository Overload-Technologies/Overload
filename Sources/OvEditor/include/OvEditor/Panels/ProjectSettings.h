/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvPhysics/Settings/CollisionLayers.h>

#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Panels/PanelWindow.h>

#include <OvTools/Filesystem/IniFile.h>

namespace OvEditor::Panels
{
	class ProjectSettings : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		ProjectSettings
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Generate a gatherer that will get the value associated to the given key
		* @param p_keyName
		*/
		template <typename T>
		std::function<T()> GenerateGatherer(const std::string& p_keyName)
		{
			return std::bind(&OvTools::Filesystem::IniFile::Get<T>, &m_projectFile, p_keyName);
		}

		/**
		* Generate a provider that will set the value associated to the given key
		* @param p_keyName
		*/
		template <typename T>
		std::function<void(T)> GenerateProvider(const std::string& p_keyName)
		{
			return std::bind(&OvTools::Filesystem::IniFile::Set<T>, &m_projectFile, p_keyName, std::placeholders::_1);
		}

	private:
		/**
		* Rebuild the widgets used to edit the collision layers
		* @param p_container
		*/
		void BuildCollisionLayerWidgets(OvUI::Internal::WidgetContainer& p_container);

		/**
		* Rebuild the widgets used to edit the collision matrix
		* @param p_container
		*/
		void BuildCollisionMatrixWidgets(OvUI::Internal::WidgetContainer& p_container);

		/**
		* Restore the collision layers from the project settings and rebuild their widgets
		*/
		void ReloadCollisionLayers();

		/**
		* Store the collision layers into the project settings
		*/
		void StoreCollisionLayers();

		OvTools::Filesystem::IniFile& m_projectFile;
		OvPhysics::Settings::CollisionLayers m_collisionLayers;

		/* Kept apart so that renaming a layer can refresh the matrix without rebuilding the
		   name field being typed into */
		OvUI::Widgets::Layout::Group* m_collisionLayersRoot = nullptr;
		OvUI::Widgets::Layout::Group* m_collisionMatrixRoot = nullptr;
	};
}