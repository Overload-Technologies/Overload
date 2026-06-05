/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string_view>

namespace baregl::debug
{
	/**
	* Base interface for custom assert handlers
	*/
	class IAssertHandler
	{
	public:
		/**
		* Destructor
		*/
		virtual ~IAssertHandler() = default;

		/**
		* Invoked when an assert fail
		* @param p_message
		*/
		virtual void OnAssert(const std::string_view p_message) = 0;
	};
}

