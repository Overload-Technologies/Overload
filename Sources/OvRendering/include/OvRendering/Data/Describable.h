/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <typeindex>
#include <any>

#include "OvRendering/HAL/Framebuffer.h"
#include "OvRendering/Entities/Camera.h"

namespace OvRendering::Data
{
	/**
	* An object that can be described using additional data structures (descriptors)
	*/
	class Describable
	{
	public:
		/**
		* Add a descriptor
		* @param p_args (Parameter pack forwared to the extension constructor)
		*/
		template<typename T>
		void AddDescriptor(T&& p_descriptor);

		/**
		* Remove a descriptor
		*/
		template<typename T>
		void RemoveDescriptor();

		/**
		* Remove all associated descriptors
		*/
		void ClearDescriptors();

		/**
		* Return true if the a descriptor matching the given type has been found
		*/
		template<typename T>
		bool HasDescriptor() const;

		/**
		* Retrieve the descriptor matching the given type
		* @note Fails if the descriptor doesn't exist
		*/
		template<typename T>
		const T& GetDescriptor() const;

		/**
		* Try retrieving the descriptor matching the given type
		* @param p_outDescriptor
		* @return true if the descriptor has been found
		*/
		template<typename T>
		bool TryGetDescriptor(OvTools::Utils::OptRef<const T>& p_outDescriptor) const;

	private:
		std::unordered_map<std::type_index, std::any> m_descriptors;
	};
}

#include "OvRendering/Data/Describable.inl"
