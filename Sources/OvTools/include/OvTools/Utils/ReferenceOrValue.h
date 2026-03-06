/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <variant>
#include <optional>
#include <stdexcept>

#include "OvTools/Utils/OptRef.h"

namespace OvTools::Utils
{
	/**
	* A simple class that can represent a reference or a value of the given type.
	* The usage of the data is the same for both usages.
	* The goal is to be able to instanciate objects that can be a reference or a value without
	* taking care of what it is.
	*/
	template <typename T>
	class ReferenceOrValue
	{
	public:
		/*
		* Construct the ReferenceOrValue instance
		* @param p_ref
		*/
		ReferenceOrValue(OptRef<T> p_ref = std::nullopt)
		{
			if (p_ref)
			{
				m_data = &p_ref.value();
			}
			else
			{
				m_data = T();
			}
		}

		/**
		* Construct the ReferenceOrValue instance
		* @param p_ref
		*/
		ReferenceOrValue(T& p_ref)
		{
			m_data = &p_ref;
		}

		/**
		* Implicit conversion of a ReferenceOrValue to a T
		*/
		operator T&()
		{
			return GetRef();
		}

		/**
		* Assignment operator thats call the setter of the ReferenceOrValue instance
		* @param p_value
		*/
		ReferenceOrValue<T>& operator=(const T& p_value)
		{
			Set(p_value);
			return *this;
		}

		/**
		* Returns the value as a reference (From reference or directly from the value)
		*/
		T& GetRef()
		{
			if (auto pval = std::get_if<T>(&m_data))
				return *pval;
			else
				return *std::get<T*>(m_data);
		}

		T const* operator->() const
		{
			return &Get();
		}

		T* operator->()
		{
			return &GetRef();
		}

		/**
		* Returns the value as a const reference (From reference or directly from the value)
		*/
		const T& Get() const
		{
			if (auto pval = std::get_if<T>(&m_data))
				return *pval;
			else
				return *std::get<T*>(m_data);
		}

		/**
		* Sets the value (To the reference or directly to the value)
		* @param p_value
		*/
		void Set(const T& p_value)
		{
			if (auto pval = std::get_if<T>(&m_data))
				*pval = p_value;
			else
				*std::get<T*>(m_data) = p_value;
		}

	private:
		std::variant<T, T*> m_data;
	};
}