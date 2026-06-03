/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/data/BufferMemoryRange.h>
#include <baregl/detail/NativeObject.h>
#include <baregl/types/EAccessSpecifier.h>
#include <baregl/types/EBufferType.h>

#include <optional>

namespace baregl
{
	/**
	* Represents a buffer, used to store data on the GPU
	*/
	class Buffer final : public detail::NativeObject
	{
	public:
		/**
		* Creates a buffer
		*/
		Buffer();

		/**
		* Destroys the buffer
		*/
		~Buffer();

		/**
		* Allocates memory for the buffer
		* @param p_size
		* @param p_usage
		* @return The size of the allocated memory in bytes
		*/
		uint64_t Allocate(uint64_t p_size, types::EAccessSpecifier p_usage = types::EAccessSpecifier::STATIC_DRAW);

		/**
		* Uploads data to the buffer
		* @param p_data
		* @param p_range
		*/
		void Upload(const void* p_data, std::optional<data::BufferMemoryRange> p_range = std::nullopt);

		/**
		* Returns true if the buffer is valid (properly allocated)
		*/
		bool IsValid() const;

		/**
		* Returns true if the buffer is empty
		*/
		bool IsEmpty() const;

		/**
		* Returns the size of the allocated buffer in bytes
		*/
		uint64_t GetSize() const;

		/**
		* Binds the buffer
		* @param p_type Type of the buffer to bind
		* @param p_index (Optional) Index to bind the buffer to
		*/
		void Bind(
			types::EBufferType p_type,
			std::optional<uint32_t> p_index = std::nullopt
		);

		/**
		* Unbinds the buffer
		*/
		void Unbind();

	protected:
		uint64_t m_allocatedBytes = 0;
		std::optional<types::EBufferType> m_boundAs = std::nullopt;
		std::optional<uint32_t> m_bindIndex = std::nullopt;
	};
}
