/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Data/Image.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image/stb_image.h>

OvRendering::Data::Image::Image(const std::filesystem::path& p_filepath, const bool p_flipVertically)
{
	stbi_set_flip_vertically_on_load(p_flipVertically ? 1 : 0);

	isHDR = stbi_is_hdr(p_filepath.string().c_str());

	data = isHDR ?
		static_cast<void*>(stbi_loadf(p_filepath.string().c_str(), &width, &height, &bpp, 4)) :
		static_cast<void*>(stbi_load(p_filepath.string().c_str(), &width, &height, &bpp, 4));
}

OvRendering::Data::Image::Image(const uint8_t* p_data, const size_t p_size, const bool p_flipVertically)
{
	if (!p_data || p_size == 0 || p_size > static_cast<size_t>(std::numeric_limits<int>::max()))
	{
		return;
	}

	const int encodedSize = static_cast<int>(p_size);
	stbi_set_flip_vertically_on_load(p_flipVertically ? 1 : 0);
	isHDR = stbi_is_hdr_from_memory(p_data, encodedSize) != 0;

	data = isHDR ?
		static_cast<void*>(stbi_loadf_from_memory(p_data, encodedSize, &width, &height, &bpp, 4)) :
		static_cast<void*>(stbi_load_from_memory(p_data, encodedSize, &width, &height, &bpp, 4));
}

OvRendering::Data::Image::~Image()
{
	stbi_image_free(data);
}

bool OvRendering::Data::Image::IsValid() const
{
	return data;
}

OvRendering::Data::Image::operator bool() const
{
	return IsValid();
}

