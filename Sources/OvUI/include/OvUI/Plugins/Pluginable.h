/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include "OvUI/Plugins/IPlugin.h"

namespace OvUI::Plugins
{
	/**
	* Inherit from this class to make your class "Pluginable" (Able to have plugins)
	*/
	class Pluginable
	{
	public:
		Pluginable() = default;
		Pluginable(const Pluginable&) = delete;
		Pluginable& operator=(const Pluginable&) = delete;
		Pluginable(Pluginable&&) = delete;
		Pluginable& operator=(Pluginable&&) = delete;

		/**
		* Destructor (Destroys every plugins)
		*/
		virtual ~Pluginable()
		{
			RemoveAllPlugins();
		}

		/**
		* Add a plugin
		* @param p_args
		*/
		template<typename T, typename... Args>
		T& AddPlugin(Args&& ... p_args)
		{
			static_assert(std::is_base_of<Plugins::IPlugin, T>::value, "T should derive from IPlugin");

			T* newPlugin = new T(std::forward<Args>(p_args)...);
			m_plugins.push_back(newPlugin);
			return *newPlugin;
		}

		/**
		* Returns the plugin of the given type, or nullptr if not found
		*/
		template<typename T>
		T* GetPlugin()
		{
			static_assert(std::is_base_of<Plugins::IPlugin, T>::value, "T should derive from IPlugin");

			for (size_t i = 0; i < m_plugins.size(); ++i)
			{
				T* result = dynamic_cast<T*>(m_plugins[i]);
				if (result)
					return result;
			}

			return nullptr;
		}

		/**
		* Execute every plugins
		*/
		void ExecutePlugins(EPluginExecutionContext p_context)
		{
			for (auto& plugin : m_plugins)
				plugin->Execute(p_context);
		}

		/**
		* Remove every plugins
		*/
		void RemoveAllPlugins()
		{
			for (auto& plugin : m_plugins)
				delete plugin;

			m_plugins.clear();
		}

	private:
		std::vector<IPlugin*> m_plugins;
	};
}
