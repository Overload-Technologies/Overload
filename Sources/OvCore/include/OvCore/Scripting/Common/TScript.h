/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Scripting/Common/EScriptingLanguage.h>

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

	protected:
		Context m_context;
	};
}
