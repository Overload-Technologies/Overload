/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

namespace OvRendering::Data
{
	/**
	* Simple wrapper for stb_image. Handles SDR and HDR image loading,
	* and enforces RAII for the loaded data.
	*/
	struct Image
	{
		int width = 0;
		int height = 0;
		int bpp = 0;
		bool isHDR = false;
		void* data = nullptr;

		/**
		* Creates an image from a file on disk
		* @param p_filepath
		* @param p_flipVertically
		*/
		Image(const std::filesystem::path& p_filepath, bool p_flipVertically = true);

		/**
		* Creates an image from encoded image data in memory
		* @param p_flipVertically
		*/
		Image(const uint8_t* p_data, const size_t p_size, bool p_flipVertically = true);

		/**
		* Destructor of the image
		*/
		virtual ~Image();

		/**
		* Returns true if the image is valid
		*/
		bool IsValid() const;

		/**
		* Alias for IsValid()
		*/
		operator bool() const;
	};
}
