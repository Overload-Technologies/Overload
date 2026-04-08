/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <map>
#include <optional>
#include <string>

#include <OvCore/Scripting/Common/EScriptingLanguage.h>
#include <OvCore/Scripting/Common/ScriptPropertyValue.h>

namespace OvCore::Scripting
{
	/**
	* Interface for any scripting backend to implement.
	* This class defines the necessary methods that a scripting engine should provide
	* to interact with the engine's components and handle various lifecycle events.
	*/
	template<EScriptingLanguage Language, class Context>
	class TScript
	{
	public:
		/**
		* Constructor of the generic script
		*/
		TScript();

		/**
		* Destructor of the generic script (virtual to allow polymorphism)
		*/
		virtual ~TScript();

		/**
		* Checks if the script context is valid.
		* @return True if the context is valid, false otherwise.
		*/
		bool IsValid() const;

		/**
		* Return the context of the script
		*/
		inline const Context& GetContext() const { return m_context; }
		inline Context& GetContext() { return m_context; }

		/**
		* Returns all primitive properties and their default values as defined by the script.
		*/
		std::map<std::string, ScriptPropertyValue> GetDefaultProperties() const;

		/**
		* Returns the live value of a named property from the script's runtime state.
		* Returns std::nullopt if the property does not exist or cannot be read.
		*/
		std::optional<ScriptPropertyValue> GetProperty(const std::string& p_key) const;

		/**
		* Sets the value of a named property in the script's runtime state.
		*/
		void SetProperty(const std::string& p_key, const ScriptPropertyValue& p_value);

	protected:
		Context m_context;
	};
}
