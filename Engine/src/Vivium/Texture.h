#pragma once

#include "Renderer.h"
#include "Logger.h"

#include <stb_image.h>

namespace Vivium {
	class Font;

	class VIVIUM_API Texture {
	public:
		enum class Format : int {
			RGB = GL_RGB,
			RGBA = GL_RGBA,
			GRAYSCALE = GL_LUMINANCE
		};

	private:
		GLuint m_ID;
		int m_Width, m_Height;

		static int m_GetInternalFormat(const Format& format);

	public:
		static std::string PATH;

		static void Unbind();
		void Bind(uint8_t slot) const;

		int GetWidth() const;
		int GetHeight() const;
		Vector2<int> GetDim() const;

		Texture(uint8_t* buffer, int width, int height, const Format& format = Format::RGBA);
		// Construct from png
		Texture(const std::string& filename);
		// Construct from font
		Texture(Font* font);

		~Texture();
	};

	// TODO
	class VIVIUM_API TextureArray {
		GLuint id;

		int width, height;

		static void Unbind();
		void Bind(uint8_t slot) const;

		TextureArray(int width, int height, int count, unsigned char* data);
		~TextureArray();
	};
}
