#pragma once

#include "Core.h"
#include "Logger.h"
#include "Texture.h"
#include "Vector2.h"
#include "Utils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

// https://learnopengl.com/In-Practice/Text-Rendering

namespace Vivium {
	class VIVIUM_API Font {
		void m_LoadChar(const FT_Face& face, char character);

	public:
		static std::string PATH;

		uint32_t max_height = 0;
		std::vector<unsigned char>* buffer = nullptr;
		uint32_t buffer_width = 128 * 64;
		uint32_t buffer_height = 64;

		struct VIVIUM_API Character {
			uint32_t texture_id;  // ID of glyph
			Vector2<int> size;	  // Size of glyph
			Vector2<int> bearing; // Offset from baseline to top left of glyph
			uint32_t advance;     // Offset to advance to next glpyh

			Character();
			Character(const Character& other);
			Character(uint32_t texture_id, Vector2<int> size, Vector2<int> bearing, uint32_t advance);
		};

		typedef std::unordered_map<char, Character> CharacterMap_t;

		CharacterMap_t character_map;
		uint32_t vertex_array_id, vertex_buffer_id;

		Font(const char* font_path);
		~Font();
	};
}
