/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/debug/IAssertHandler.h>
#include <baregl/debug/ILogHandler.h>
#include <baregl/debug/IEventHandler.h>

#include <memory>

namespace baregl::debug
{
	/**
	* Provides an assert handler for baregl internal asserts
	* @param p_handler
	*/
	void SetAssertHandler(std::unique_ptr<IAssertHandler> p_handler);

	/**
	* Provides a log handler for baregl internal logs
	* @param p_handler
	*/
	void SetLogHandler(std::unique_ptr<ILogHandler> p_handler);

	/**
	* Provides an event handler for baregl notifications
	* @param p_handler
	*/
	void SetEventHandler(std::unique_ptr<IEventHandler> p_handler);
}

