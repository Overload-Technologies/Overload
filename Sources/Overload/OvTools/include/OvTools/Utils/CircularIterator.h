/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <span>

namespace OvTools::Utils
{
	/**
	* Helper class to iterate over a circular buffer on size n
	*/
	template<class T, uint8_t Size>
	class CircularIterator
	{
	public:
		/**
		* Constructor of the CircularIterator class
		* @param p_data
		*/
		CircularIterator(std::span<T, Size> p_data) : m_data(p_data) {}

		/**
		* Return the element at the given offset
		*/
		T& operator[](uint8_t offset)
		{
			return m_data[static_cast<size_t>(m_currentIndex + offset) % Size];
		}

		/**
		* Increment the current index circularly.
		*/
		void operator++()
		{
			m_currentIndex = (m_currentIndex + 1) % Size;
		}

		/**
		* Return the size of the circular buffer
		*/
		constexpr uint8_t GetSize() const
		{
			return Size;
		}

	private:
		uint8_t m_currentIndex = 0;
		std::span<T, Size> m_data;
	};
}
