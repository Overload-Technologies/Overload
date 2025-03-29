/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvTools/Filesystem/IniFile.h>
#include <OvTools/Utils/String.h>

#include <fstream>

namespace
{
	std::pair<std::string, std::string> ExtractKeyAndValue(std::string_view p_line)
	{
		std::string key;
		std::string value;

		std::string* currentBuffer = &key;

		for (const char c : p_line)
		{
			if (c == '=')
				currentBuffer = &value;
			else
				currentBuffer->push_back(c);
		}

		return { key, value };
	}

	bool IsValidLine(std::string_view p_attributeLine)
	{
		if (p_attributeLine.size() == 0)
			return false;

		if (p_attributeLine[0] == '#' || p_attributeLine[0] == ';' || p_attributeLine[0] == '[')
			return false;

		if (std::count(p_attributeLine.begin(), p_attributeLine.end(), '=') != 1)
			return false;

		return true;
	}
}

OvTools::Filesystem::IniFile::IniFile(const std::string& p_filePath) : m_filePath(p_filePath)
{
	Load();
}

void OvTools::Filesystem::IniFile::Reload()
{
	RemoveAll();
	Load();
}

bool OvTools::Filesystem::IniFile::Remove(const std::string & p_key)
{
	if (IsKeyExisting(p_key))
	{
		m_data.erase(p_key);
		return true;
	}

	return false;
}

void OvTools::Filesystem::IniFile::RemoveAll()
{
	m_data.clear();
}

bool OvTools::Filesystem::IniFile::IsKeyExisting(const std::string& p_key) const
{
	return m_data.find(p_key) != m_data.end();
}

void OvTools::Filesystem::IniFile::RegisterPair(const std::string& p_key, const std::string& p_value)
{
	RegisterPair(std::make_pair(p_key, p_value));
}

void OvTools::Filesystem::IniFile::RegisterPair(const AttributePair& p_pair)
{
	m_data.insert(p_pair);
}

void OvTools::Filesystem::IniFile::Load()
{
	std::fstream iniFile;
	iniFile.open(m_filePath);

	if (iniFile.is_open())
	{
		std::string currentLine;

		while (std::getline(iniFile, currentLine))
		{
			if (IsValidLine(currentLine))
			{
				OvTools::Utils::String::Trim(currentLine);
				RegisterPair(ExtractKeyAndValue(currentLine));
			}
		}

		iniFile.close();
	}
}

void OvTools::Filesystem::IniFile::Rewrite() const
{
	std::ofstream outfile;
	outfile.open(m_filePath, std::ios_base::trunc);

	if (outfile.is_open())
	{
		for (const auto& [key, value] : m_data)
		{
			outfile << key << "=" << value << std::endl;
		}
	}

	outfile.close();
}
