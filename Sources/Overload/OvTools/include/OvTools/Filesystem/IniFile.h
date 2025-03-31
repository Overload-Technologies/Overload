/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <unordered_map>

namespace OvTools::Filesystem
{
	template<typename T>
	concept SupportedIniType =
		std::same_as<T, bool> ||
		std::same_as<T, std::string> ||
		std::integral<T> ||
		std::floating_point<T>;

	/**
	* The IniFile class represents a file .ini that stores a set of attributes/values that can get read and written
	*/
	class IniFile final
	{
	public:
		using AttributePair = std::pair<std::string, std::string>;
		using AttributeMap = std::unordered_map<std::string, std::string>;

		/**
		* Create an IniFile by parsing the given file path and extracting key/values pairs for future usage
		* @param p_filePath
		*/
		IniFile(const std::string& p_filePath);

		/**
		* Overwrite the content of the current data by reloading the file
		*/
		void Reload();

		/**
		* Rewrite the entiere .ini file with the current values. This operation is destructive and can't be undone.
		* Any comment or line break in your .ini file will get destroyed
		*/
		void Rewrite() const;

		/**
		* Return the value attached to the given key
		* If the key doesn't exist, a default value is returned
		* @param p_key
		*/
		template<SupportedIniType T>
		T Get(const std::string& p_key);

		/**
		* Return the value attached to the given key
		* If the key doesn't exist, the specified value is returned
		* @param p_key
		* @param p_default
		*/
		template<SupportedIniType T>
		T GetOrDefault(const std::string& p_key, T p_default);

		/**
		* Try to get the value attached to the given key
		* @param p_key
		* @param p_outValue
		*/
		template<SupportedIniType T>
		bool TryGet(const std::string& p_key, T& p_outValue);

		/**
		* Set a new value to the given key (Not applied to the real file untill Rewrite() or Save() is called)
		* @param p_key
		* @param p_value
		*/
		template<SupportedIniType T>
		bool Set(const std::string& p_key, const T& p_value);

		/**
		* Add a new key/value to the IniFile object (Not applied to the real file untill Rewrite() or Save() is called)
		* @param p_key
		* @param p_value
		*/
		template<SupportedIniType T>
		bool Add(const std::string& p_key, const T& p_value);

		/**
		* Remove an key/value pair identified by the given key (Not applied to the real file untill Rewrite() or Save() is called)
		* @param p_key
		*/
		bool Remove(const std::string& p_key);

		/**
		* Remove all key/value pairs (Not applied to the real file untill Rewrite() or Save() is called)
		*/
		void RemoveAll();

		/**
		* Verify if the given key exists
		* @param p_key
		*/
		bool IsKeyExisting(const std::string& p_key) const;

	private:
		void RegisterPair(const std::string& p_key, const std::string& p_value);
		void RegisterPair(const AttributePair& p_pair);

		void Load();

	private:
		std::string m_filePath;
		AttributeMap m_data;
	};
}

#include "OvTools/Filesystem/IniFile.inl"
