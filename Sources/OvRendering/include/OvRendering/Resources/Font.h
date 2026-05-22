/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

namespace OvRendering::Resources
{
	class Texture;

	/**
	* Font resource baked into a static glyph atlas
	*/
	class Font
	{
	public:
		struct Glyph
		{
			float xOffset = 0.0f;
			float yOffset = 0.0f;
			float xAdvance = 0.0f;
			float width = 0.0f;
			float height = 0.0f;
			float uMin = 0.0f;
			float vMin = 0.0f;
			float uMax = 0.0f;
			float vMax = 0.0f;
		};

		static constexpr uint32_t kFirstGlyph = 32;
		static constexpr uint32_t kGlyphCount = 95;

		/**
		* Constructor
		* @param p_path
		* @param p_realPath
		*/
		Font(const std::string& p_path, const std::filesystem::path& p_realPath);

		/**
		* Destructor
		*/
		~Font();

		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

		/**
		* Reloads the font from disk
		* @param p_realPath
		*/
		bool Reload(const std::filesystem::path& p_realPath);

		/**
		* Returns true if the font has a valid atlas
		*/
		bool IsValid() const;

		/**
		* Returns the glyph atlas font size
		*/
		float GetPixelSize() const;

		/**
		* Returns the line height in atlas pixels
		*/
		float GetLineHeight() const;

		/**
		* Returns the glyph associated with the given ASCII character
		* @param p_character
		*/
		const Glyph* GetGlyph(char p_character) const;

		/**
		* Returns the static glyph atlas texture
		*/
		Texture* GetAtlasTexture() const;

	public:
		const std::string path;

	private:
		bool m_valid = false;
		float m_pixelSize = 32.0f;
		float m_lineHeight = 32.0f;
		std::array<Glyph, kGlyphCount> m_glyphs = {};
		Texture* m_atlasTexture = nullptr;
	};
}
