/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstring>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#endif

inline bool OvTools::Utils::SetWindowsExecutableIcon(
	const std::filesystem::path& p_executablePath,
	const std::filesystem::path& p_iconPath
)
{
#if defined(_WIN32)
	constexpr WORD kGroupResourceId = 101;
	constexpr WORD kImageResourceId = 65000;
	constexpr UINT kMaxIconSize = 256;
	auto writeWord = [](std::vector<BYTE>& p_data, const size_t p_offset, const WORD p_value)
	{
		p_data[p_offset + 0u] = static_cast<BYTE>(p_value & 0xFFu);
		p_data[p_offset + 1u] = static_cast<BYTE>((p_value >> 8u) & 0xFFu);
	};
	auto writeDword = [](std::vector<BYTE>& p_data, const size_t p_offset, const DWORD p_value)
	{
		p_data[p_offset + 0u] = static_cast<BYTE>(p_value & 0xFFu);
		p_data[p_offset + 1u] = static_cast<BYTE>((p_value >> 8u) & 0xFFu);
		p_data[p_offset + 2u] = static_cast<BYTE>((p_value >> 16u) & 0xFFu);
		p_data[p_offset + 3u] = static_cast<BYTE>((p_value >> 24u) & 0xFFu);
	};

	ULONG_PTR gdiplusToken = 0;
	Gdiplus::GdiplusStartupInput startupInput;
	if (Gdiplus::GdiplusStartup(&gdiplusToken, &startupInput, nullptr) != Gdiplus::Ok)
	{
		return false;
	}

	const bool success = [&]() -> bool
	{
		Gdiplus::Bitmap sourceBitmap(p_iconPath.wstring().c_str());
		if (sourceBitmap.GetLastStatus() != Gdiplus::Ok)
		{
			return false;
		}

		const UINT iconWidth = sourceBitmap.GetWidth() < kMaxIconSize ? sourceBitmap.GetWidth() : kMaxIconSize;
		const UINT iconHeight = sourceBitmap.GetHeight() < kMaxIconSize ? sourceBitmap.GetHeight() : kMaxIconSize;
		if (iconWidth == 0u || iconHeight == 0u)
		{
			return false;
		}

		Gdiplus::Bitmap iconBitmap(iconWidth, iconHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics graphics(&iconBitmap);
		if (iconBitmap.GetLastStatus() != Gdiplus::Ok)
		{
			return false;
		}
		if (graphics.DrawImage(&sourceBitmap, 0, 0, iconWidth, iconHeight) != Gdiplus::Ok)
		{
			return false;
		}

		Gdiplus::Rect lockRect(0, 0, static_cast<INT>(iconWidth), static_cast<INT>(iconHeight));
		Gdiplus::BitmapData bitmapData{};
		if (iconBitmap.LockBits(&lockRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok)
		{
			return false;
		}

		const int stride = bitmapData.Stride < 0 ? -bitmapData.Stride : bitmapData.Stride;
		const size_t rowBytes = static_cast<size_t>(iconWidth) * 4u;
		std::vector<BYTE> xorBitmapData(rowBytes * static_cast<size_t>(iconHeight));
		for (UINT y = 0u; y < iconHeight; ++y)
		{
			const size_t dstOffset = static_cast<size_t>(iconHeight - 1u - y) * rowBytes;
			const auto src = static_cast<const BYTE*>(bitmapData.Scan0) + static_cast<size_t>(y) * static_cast<size_t>(stride);
			std::memcpy(xorBitmapData.data() + dstOffset, src, rowBytes);
		}
		iconBitmap.UnlockBits(&bitmapData);

		const size_t andMaskRowBytes = static_cast<size_t>((iconWidth + 31u) / 32u) * 4u;
		std::vector<BYTE> andMaskData(andMaskRowBytes * static_cast<size_t>(iconHeight), 0u);
		BITMAPINFOHEADER iconHeader{};
		iconHeader.biSize = sizeof(BITMAPINFOHEADER);
		iconHeader.biWidth = static_cast<LONG>(iconWidth);
		iconHeader.biHeight = static_cast<LONG>(iconHeight * 2u);
		iconHeader.biPlanes = 1;
		iconHeader.biBitCount = 32;
		iconHeader.biCompression = BI_RGB;
		iconHeader.biSizeImage = static_cast<DWORD>(xorBitmapData.size());

		std::vector<BYTE> iconResourceData(sizeof(BITMAPINFOHEADER) + xorBitmapData.size() + andMaskData.size());
		std::memcpy(iconResourceData.data(), &iconHeader, sizeof(BITMAPINFOHEADER));
		std::memcpy(iconResourceData.data() + sizeof(BITMAPINFOHEADER), xorBitmapData.data(), xorBitmapData.size());
		std::memcpy(iconResourceData.data() + sizeof(BITMAPINFOHEADER) + xorBitmapData.size(), andMaskData.data(), andMaskData.size());

		std::vector<BYTE> groupResourceData(20u, 0u);
		writeWord(groupResourceData, 0u, 0u);
		writeWord(groupResourceData, 2u, 1u);
		writeWord(groupResourceData, 4u, 1u);
		groupResourceData[6u] = iconWidth >= kMaxIconSize ? 0u : static_cast<BYTE>(iconWidth);
		groupResourceData[7u] = iconHeight >= kMaxIconSize ? 0u : static_cast<BYTE>(iconHeight);
		writeWord(groupResourceData, 10u, 1u);
		writeWord(groupResourceData, 12u, 32u);
		writeDword(groupResourceData, 14u, static_cast<DWORD>(iconResourceData.size()));
		writeWord(groupResourceData, 18u, kImageResourceId);

		const std::wstring executablePath = p_executablePath.wstring();
		HANDLE updateHandle = BeginUpdateResourceW(executablePath.c_str(), FALSE);
		if (!updateHandle)
		{
			return false;
		}

		if (!UpdateResourceW(updateHandle, MAKEINTRESOURCEW(3), MAKEINTRESOURCEW(kImageResourceId), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN), iconResourceData.data(), static_cast<DWORD>(iconResourceData.size()))
			|| !UpdateResourceW(updateHandle, MAKEINTRESOURCEW(14), MAKEINTRESOURCEW(kGroupResourceId), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN), groupResourceData.data(), static_cast<DWORD>(groupResourceData.size())))
		{
			EndUpdateResourceW(updateHandle, TRUE);
			return false;
		}

		return EndUpdateResourceW(updateHandle, FALSE) != FALSE;
	}();

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return success;
#else
	(void)p_executablePath;
	(void)p_iconPath;
	return false;
#endif
}
