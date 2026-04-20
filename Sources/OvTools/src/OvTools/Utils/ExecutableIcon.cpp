/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

#include <OvTools/Utils/ExecutableIcon.h>

#ifdef _WIN32
#include <stb_image/stb_image.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace
{
#ifdef _WIN32
	constexpr WORD kApplicationIconResourceID = 101;
	constexpr WORD kApplicationIconImageID = 1;
	constexpr WORD kIconResourceTypeID = 3;
	constexpr WORD kGroupIconResourceTypeID = 14;

	void WriteUint16(std::vector<uint8_t>& p_data, size_t p_offset, uint16_t p_value)
	{
		p_data[p_offset] = static_cast<uint8_t>(p_value & 0xFF);
		p_data[p_offset + 1] = static_cast<uint8_t>((p_value >> 8) & 0xFF);
	}

	void WriteUint32(std::vector<uint8_t>& p_data, size_t p_offset, uint32_t p_value)
	{
		p_data[p_offset] = static_cast<uint8_t>(p_value & 0xFF);
		p_data[p_offset + 1] = static_cast<uint8_t>((p_value >> 8) & 0xFF);
		p_data[p_offset + 2] = static_cast<uint8_t>((p_value >> 16) & 0xFF);
		p_data[p_offset + 3] = static_cast<uint8_t>((p_value >> 24) & 0xFF);
	}

	void WriteInt32(std::vector<uint8_t>& p_data, size_t p_offset, int32_t p_value)
	{
		WriteUint32(p_data, p_offset, static_cast<uint32_t>(p_value));
	}

	std::string GetSystemErrorMessage(DWORD p_errorCode)
	{
		if (p_errorCode == 0)
		{
			return "Unknown error";
		}

		LPSTR buffer = nullptr;
		const DWORD size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			p_errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&buffer),
			0,
			nullptr
		);

		if (size == 0 || buffer == nullptr)
		{
			return "Win32 error code: " + std::to_string(p_errorCode);
		}

		std::string message(buffer, size);
		LocalFree(buffer);

		while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
		{
			message.pop_back();
		}

		return message;
	}

	bool BuildIconResources(const std::filesystem::path& p_iconPath, std::vector<uint8_t>& p_iconData, std::vector<uint8_t>& p_groupIconData, std::string& p_error)
	{
		int width = 0;
		int height = 0;

		std::ifstream iconFile(p_iconPath, std::ios::binary | std::ios::ate);
		if (!iconFile)
		{
			p_error = "Failed to open icon image file: " + p_iconPath.string();
			return false;
		}

		const auto iconFileSize = iconFile.tellg();
		if (iconFileSize <= 0)
		{
			p_error = "Icon image file is empty: " + p_iconPath.string();
			return false;
		}

		std::vector<uint8_t> iconFileData(static_cast<size_t>(iconFileSize));
		iconFile.seekg(0, std::ios::beg);

		if (!iconFile.read(reinterpret_cast<char*>(iconFileData.data()), static_cast<std::streamsize>(iconFileData.size())))
		{
			p_error = "Failed to read icon image file: " + p_iconPath.string();
			return false;
		}

		if (iconFileData.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
		{
			p_error = "Icon image file is too large: " + p_iconPath.string();
			return false;
		}

		unsigned char* pixels = stbi_load_from_memory(
			iconFileData.data(),
			static_cast<int>(iconFileData.size()),
			&width,
			&height,
			nullptr,
			4
		);

		if (!pixels)
		{
			p_error = "Failed to load icon image file: " + p_iconPath.string();
			return false;
		}

		if (width <= 0 || height <= 0 || width > 256 || height > 256)
		{
			stbi_image_free(pixels);
			p_error = "Icon image dimensions must be between 1 and 256 pixels: " + p_iconPath.string();
			return false;
		}

		const uint8_t iconWidth = static_cast<uint8_t>(width == 256 ? 0 : width);
		const uint8_t iconHeight = static_cast<uint8_t>(height == 256 ? 0 : height);
		constexpr size_t kBitmapInfoHeaderSize = 40;
		constexpr size_t kGroupIconHeaderSize = 6;
		constexpr size_t kGroupIconEntrySize = 14;
		const size_t xorMaskBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * sizeof(uint32_t);
		const size_t andMaskStride = static_cast<size_t>(((width + 31) / 32) * 4);
		const size_t andMaskBytes = andMaskStride * static_cast<size_t>(height);
		const size_t totalSize = kBitmapInfoHeaderSize + xorMaskBytes + andMaskBytes;

		p_iconData.assign(totalSize, 0);
		WriteUint32(p_iconData, 0, static_cast<uint32_t>(kBitmapInfoHeaderSize));
		WriteInt32(p_iconData, 4, width);
		WriteInt32(p_iconData, 8, height * 2); // Includes XOR + AND masks
		WriteUint16(p_iconData, 12, 1);
		WriteUint16(p_iconData, 14, 32);
		WriteUint32(p_iconData, 16, 0); // BI_RGB
		WriteUint32(p_iconData, 20, static_cast<uint32_t>(xorMaskBytes));

		for (int y = 0; y < height; ++y)
		{
			const int sourceY = height - 1 - y; // DIB is bottom-up

			for (int x = 0; x < width; ++x)
			{
				const size_t sourceIndex = (static_cast<size_t>(sourceY) * static_cast<size_t>(width) + static_cast<size_t>(x)) * 4;
				const size_t destinationIndex = kBitmapInfoHeaderSize + (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)) * 4;

				const uint8_t r = pixels[sourceIndex + 0];
				const uint8_t g = pixels[sourceIndex + 1];
				const uint8_t b = pixels[sourceIndex + 2];
				const uint8_t a = pixels[sourceIndex + 3];

				p_iconData[destinationIndex + 0] = b;
				p_iconData[destinationIndex + 1] = g;
				p_iconData[destinationIndex + 2] = r;
				p_iconData[destinationIndex + 3] = a;
			}
		}

		stbi_image_free(pixels);

		p_groupIconData.assign(kGroupIconHeaderSize + kGroupIconEntrySize, 0);
		WriteUint16(p_groupIconData, 0, 0);
		WriteUint16(p_groupIconData, 2, 1);
		WriteUint16(p_groupIconData, 4, 1);
		p_groupIconData[6] = iconWidth;
		p_groupIconData[7] = iconHeight;
		p_groupIconData[8] = 0;
		p_groupIconData[9] = 0;
		WriteUint16(p_groupIconData, 10, 1);
		WriteUint16(p_groupIconData, 12, 32);
		WriteUint32(p_groupIconData, 14, static_cast<uint32_t>(p_iconData.size()));
		WriteUint16(p_groupIconData, 18, kApplicationIconImageID);

		return true;
	}
#endif
}

bool OvTools::Utils::ChangeExecutableIcon(
	const std::filesystem::path& p_executablePath,
	const std::filesystem::path& p_iconPath,
	std::string& p_error
)
{
#if defined(_WIN32)
	std::vector<uint8_t> iconData;
	std::vector<uint8_t> groupIconData;

	if (!BuildIconResources(p_iconPath, iconData, groupIconData, p_error))
	{
		return false;
	}

	const std::wstring executablePath = p_executablePath.wstring();
	HANDLE updateHandle = BeginUpdateResourceW(executablePath.c_str(), FALSE);

	if (updateHandle == nullptr)
	{
		p_error = "BeginUpdateResource failed: " + GetSystemErrorMessage(GetLastError());
		return false;
	}

	const WORD language = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

	if (!UpdateResourceW(
		updateHandle,
		MAKEINTRESOURCEW(kIconResourceTypeID),
		MAKEINTRESOURCEW(kApplicationIconImageID),
		language,
		iconData.data(),
		static_cast<DWORD>(iconData.size())
	))
	{
		p_error = "UpdateResource (RT_ICON) failed: " + GetSystemErrorMessage(GetLastError());
		EndUpdateResourceW(updateHandle, TRUE);
		return false;
	}

	if (!UpdateResourceW(
		updateHandle,
		MAKEINTRESOURCEW(kGroupIconResourceTypeID),
		MAKEINTRESOURCEW(kApplicationIconResourceID),
		language,
		groupIconData.data(),
		static_cast<DWORD>(groupIconData.size())
	))
	{
		p_error = "UpdateResource (RT_GROUP_ICON) failed: " + GetSystemErrorMessage(GetLastError());
		EndUpdateResourceW(updateHandle, TRUE);
		return false;
	}

	if (!EndUpdateResourceW(updateHandle, FALSE))
	{
		p_error = "EndUpdateResource failed: " + GetSystemErrorMessage(GetLastError());
		return false;
	}

	p_error.clear();
	return true;
#else
	(void)p_executablePath;
	(void)p_iconPath;
	p_error.clear();
	return true;
#endif
}
