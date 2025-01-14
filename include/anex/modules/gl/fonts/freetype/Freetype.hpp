#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <anex/File.hpp>
#include <anex/glm.hpp>
#include <anex/modules/gl/textures/Texture.hpp>
namespace anex::modules::gl
{
	struct GLWindow;
}
namespace anex::modules::gl::fonts::freetype
{
	struct FreetypeFont;
	struct FreetypeCharacter
	{
		FT_UInt glyphIndex;
		std::shared_ptr<textures::Texture> texturePointer;
		glm::ivec2 size;
		glm::ivec2 bearing;
		unsigned int advance;
		FreetypeCharacter(GLWindow &window, const FreetypeFont &freeTypeFont, const float &codepoint, const float &fontSize);
	};
	struct FreetypeFont
  {
		static FT_Library freetypeLibrary;
    static bool freetypeLoaded;
    std::shared_ptr<FT_Face> facePointer;
		std::shared_ptr<int8_t[]> fontFileBytes;
		std::unordered_map<float, std::unordered_map<float, FreetypeCharacter>> codepointFontSizeCharacters;
		GLWindow &window;
    FreetypeFont(GLWindow &window, File &fontFile);
		const glm::vec2 stringSize(const std::string &string, const float &fontSize, float &lineHeight, const glm::vec2 &bounds);
		void stringToTexture(const std::string &string,
												 const glm::vec4 &color,
												 const float &fontSize,
												 float &lineHeight,
												 const glm::vec2 &textureSize,
												 std::shared_ptr<textures::Texture> &texturePointer,
												 const int64_t &cursorIndex,
												 glm::vec3 &cursorPosition);
		FreetypeCharacter &getCharacter(const float &codepoint, const float &fontSize);
		static void FT_PRINT_AND_THROW_ERROR(const FT_Error &error);
  };
}