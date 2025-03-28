/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>

#include <OvRendering/Settings/EGraphicsBackend.h>

namespace OvRendering::HAL
{
	/**
	* Represents a texture handle, acts as a view to the texture.
	*/
	template<Settings::EGraphicsBackend Backend, class Context>
	class TTextureHandle
	{
	public:
		/**
		* Binds the texture to the given slot.
		* @param p_slot Optional slot to bind the texture to.
		*/
		void Bind(std::optional<uint32_t> p_slot = std::nullopt) const;

		/**
		* Unbinds the texture.
		*/
		void Unbind() const;

		/**
		* Returns the ID associated with the texture.
		* @return The texture ID.
		*/
		uint32_t GetID() const;

	protected:
		TTextureHandle();
		TTextureHandle(uint32_t p_id);

	protected:
		Context m_context;
	};
}
