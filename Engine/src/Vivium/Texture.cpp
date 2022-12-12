#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Font.h"

namespace Vivium {
	std::string Texture::PATH = "";

	int Texture::m_GetInternalFormat(const Format& format)
	{
		switch (format) {
		case Format::RGB: return GL_RGB8;
		case Format::RGBA: return GL_RGBA8;
		case Format::GRAYSCALE: return GL_LUMINANCE8;
		default: LogWarn("Invalid format passed to m_GetInternalFormat"); return GL_RGBA8;
		}
	}

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	void Texture::Bind(uint8_t slot) const
	{
		if (slot > Renderer::MAX_TEXTURE_SLOT) { LogError("Attempted to bind texture to invalid slot: {}", (int)slot); }
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	int Texture::GetWidth() const
	{
		return m_Width;
	}

	int Texture::GetHeight() const
	{
		return m_Height;
	}

	Vector2<int> Texture::GetDim() const
	{
		return Vector2<int>(m_Width, m_Height);
	}

	Texture::Texture(uint8_t* buffer, int width, int height, const Format& format)
		: m_Width(width), m_Height(height), m_ID(0)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, Texture::m_GetInternalFormat(format), width, height, 0, (int)format, GL_UNSIGNED_BYTE, buffer);
	}

	Texture::Texture(const std::string& filename)
		: m_ID(0)
	{
		std::string full_path = PATH + filename;

		// Load pixel data into our buffer, and set width/height of image
		int bpp;
		uint8_t* image_data = stbi_load(full_path.c_str(), &m_Width, &m_Height, &bpp, STBI_rgb_alpha);

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		stbi_image_free(image_data);
	}

	Texture::Texture(Font* font)
		: m_Width(font->buffer_width), m_Height(font->buffer_height)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->buffer_width, font->buffer_height, 0, GL_RED, GL_UNSIGNED_BYTE, &font->buffer->at(0));
	}

	Texture::~Texture() {
		glDeleteTextures(1, &m_ID);
	}

	void TextureArray::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, NULL);
	}

	void TextureArray::Bind(uint8_t slot) const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	}

	TextureArray::TextureArray(int width, int height, int count, unsigned char* data)
		: width(width), height(height)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);

		glTexImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			GL_RGBA8,
			width,
			height,
			count,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			(void*)data
		);
	}

	TextureArray::~TextureArray()
	{
	}
}