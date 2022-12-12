#include "Font.h"

namespace Vivium {
	std::string Font::PATH = "";

	void Font::m_LoadChar(const FT_Face& face, char character)
	{
		if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
			LogFatal("Failed to load glyph");
		}
	}

	Font::Font(const char* font_path) {
		// TODO: initialising entire library every time seems bad
		// Initialise FT library
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			LogFatal("Couldn't initialise FreeType library");
		}

		// Initialise font
		FT_Face face;
		if (FT_New_Face(ft, (PATH + font_path).c_str(), 0, &face)) {
			LogFatal("Failed to load font");
		}

		// TODO: non-set font size
		FT_Set_Pixel_Sizes(face, 0, 64); // Extract a font size of 64

		buffer = new std::vector<unsigned char>(128 * 64 * 64, 0);
		int buffer_offset = 0;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		// Load 128 ASCII characters
		for (unsigned char c = 0; c < 128; c++)
		{
			m_LoadChar(face, c);

			uint32_t tmp_height = face->glyph->bitmap.rows;

			if (tmp_height > max_height) max_height = tmp_height;

			// https://stackoverflow.com/questions/27270822/rendering-fonts-with-freetype-and-opengl
			{
				int advance = face->glyph->advance.x >> 6;
				int bW = face->glyph->bitmap.width;
				int bH = face->glyph->bitmap.rows;

				for (int h = 0; h < bH; ++h) {
					for (int w = 0; w < bW; ++w) {
						unsigned char value = face->glyph->bitmap.buffer[w + bW * h];

						buffer->at(h * (128 * 64) + buffer_offset + w) = value;
					}
				}

				buffer_offset += 64;
			}

			// Utils::SaveAsBitmap(&buffer->at(0), buffer_width, buffer_height, 1, "../Resources/image.bmp");

			// Generate texture
			uint32_t id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width, face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Store character
			Character character = Character(
				id,
				Vector2<int>(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				Vector2<int>(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			);
			character_map[c] = character;
		}

		// Unbind texture
		glBindTexture(GL_TEXTURE_2D, NULL);

		// Free FreeType memory
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		// Setup vertex array and vertex buffer for texture quads
		glGenVertexArrays(1, &vertex_array_id);
		glGenBuffers(1, &vertex_buffer_id);
		glBindVertexArray(vertex_array_id);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindVertexArray(NULL);
	}

	Font::~Font()
	{
		delete buffer;
	}

	Font::Character::Character(uint32_t texture_id, Vector2<int> size, Vector2<int> bearing, uint32_t advance)
		: texture_id(texture_id), size(size), bearing(bearing), advance(advance) {}

	Font::Character::Character()
		: texture_id(0), size(), bearing(), advance(0) {}

	Font::Character::Character(const Character& other)
		: texture_id(other.texture_id), size(other.size), bearing(other.bearing), advance(other.advance) {}
}