#include "TextureAtlas.h"

// TODO: so much repeated code

namespace Vivium {
	void TextureAtlas::Set(Quad* quad, int index) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int y_index = index / m_AtlasDimRelative.x;
		int x_index = index - y_index * m_AtlasDimRelative.x;
		y_index = m_AtlasDimRelative.y - 1 - y_index; // Flip vertically TODO: variable for this

		// Calculate faces
		float left = x_index * inv_width * m_SpriteSize.x;
		float right = (x_index + 1) * inv_width * m_SpriteSize.x;
		float bottom = y_index * inv_height * m_SpriteSize.y;
		float top = (y_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::vector<float> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		quad->vb->Set(1, &tex_coords[0], sizeof(float) * 8);
		quad->m_TexCoords = tex_coords;
	}

	void TextureAtlas::Set(Quad* quad, const Vector2<int>& index) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int y_index = m_AtlasDimRelative.y - 1 - index.y;

		// Calculate faces
		float left = index.x * inv_width * m_SpriteSize.x;
		float right = (index.x + 1) * inv_width * m_SpriteSize.x;
		float bottom = y_index * inv_height * m_SpriteSize.y;
		float top = (y_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::vector<float> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		quad->vb->Set(1, &tex_coords[0], sizeof(float) * 8);
		quad->m_TexCoords = tex_coords;
	}

	void TextureAtlas::Set(Quad* quad, const Vector2<int>& top_left, const Vector2<int>& bottom_right) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int bot_index = m_AtlasDimRelative.y - 1 - bottom_right.y;
		int top_index = m_AtlasDimRelative.y - 1 - top_left.y;

		// Calculate faces
		float left = top_left.x * inv_width * m_SpriteSize.x;
		float right = (bottom_right.x + 1) * inv_width * m_SpriteSize.x;
		float bottom = bot_index * inv_height * m_SpriteSize.y;
		float top = (top_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::vector<float> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		quad->vb->Set(1, &tex_coords[0], sizeof(float) * 8);
		quad->m_TexCoords = tex_coords;
	}

	void TextureAtlas::m_Construct()
	{
		m_AtlasDimRelative = Vector2<int>(m_Atlas->GetWidth() / m_SpriteSize.x, m_Atlas->GetHeight() / m_SpriteSize.y);
	}

	std::vector<float> TextureAtlas::GetCoords(int index) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int y_index = index / m_AtlasDimRelative.x;
		int x_index = index - y_index * m_AtlasDimRelative.x;
		y_index = m_AtlasDimRelative.y - 1 - y_index; // Flip vertically TODO: variable for this

		// Calculate faces
		float left = x_index * inv_width * m_SpriteSize.x;
		float right = (x_index + 1) * inv_width * m_SpriteSize.x;
		float bottom = y_index * inv_height * m_SpriteSize.y;
		float top = (y_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::vector<float> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		return tex_coords;
	}

	std::vector<float> TextureAtlas::GetCoords(const Vector2<int>& index) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int y_index = m_AtlasDimRelative.y - 1 - index.y;

		// Calculate faces
		float left = index.x * inv_width * m_SpriteSize.x;
		float right = (index.x + 1) * inv_width * m_SpriteSize.x;
		float bottom = y_index * inv_height * m_SpriteSize.y;
		float top = (y_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::vector<float> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		return tex_coords;
	}

	std::vector<float> TextureAtlas::GetCoords(const Vector2<int>& top_left, const Vector2<int>& bottom_right) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int bot_index = m_AtlasDimRelative.y - 1 - bottom_right.y;
		int top_index = m_AtlasDimRelative.y - 1 - top_left.y;

		// Calculate faces
		float left = top_left.x * inv_width * m_SpriteSize.x;
		float right = (bottom_right.x + 1) * inv_width * m_SpriteSize.x;
		float bottom = bot_index * inv_height * m_SpriteSize.y;
		float top = (top_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::vector<float> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		return tex_coords;
	}

	std::array<float, 8> TextureAtlas::GetCoordsArray(int index) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int y_index = index / m_AtlasDimRelative.x;
		int x_index = index - y_index * m_AtlasDimRelative.x;
		y_index = m_AtlasDimRelative.y - 1 - y_index; // Flip vertically TODO: variable for this

		// Calculate faces
		float left = x_index * inv_width * m_SpriteSize.x;
		float right = (x_index + 1) * inv_width * m_SpriteSize.x;
		float bottom = y_index * inv_height * m_SpriteSize.y;
		float top = (y_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::array<float, 8> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		return tex_coords;
	}

	std::array<float, 8> TextureAtlas::GetCoordsArray(const Vector2<int>& index) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int y_index = m_AtlasDimRelative.y - 1 - index.y;

		// Calculate faces
		float left = index.x * inv_width * m_SpriteSize.x;
		float right = (index.x + 1) * inv_width * m_SpriteSize.x;
		float bottom = y_index * inv_height * m_SpriteSize.y;
		float top = (y_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::array<float, 8> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		return tex_coords;
	}

	std::array<float, 8> TextureAtlas::GetCoordsArray(const Vector2<int>& top_left, const Vector2<int>& bottom_right) const
	{
		// Inverse width and height of atlas
		float inv_width = 1.0f / m_Atlas->GetWidth();
		float inv_height = 1.0f / m_Atlas->GetHeight();

		int bot_index = m_AtlasDimRelative.y - 1 - bottom_right.y;
		int top_index = m_AtlasDimRelative.y - 1 - top_left.y;

		// Calculate faces
		float left = top_left.x * inv_width * m_SpriteSize.x;
		float right = (bottom_right.x + 1) * inv_width * m_SpriteSize.x;
		float bottom = bot_index * inv_height * m_SpriteSize.y;
		float top = (top_index + 1) * inv_height * m_SpriteSize.y;

		// Create tex coords list
		std::array<float, 8> tex_coords = {
				left, bottom,
				right, bottom,
				right, top,
				left, top
		};

		// Update texture coords
		return tex_coords;
	}

	Vector2<int> TextureAtlas::GetVectorIndex(int index) const
	{
		int y_index = index / m_AtlasDimRelative.x;
		int x_index = index - y_index * m_AtlasDimRelative.x;
		y_index = m_AtlasDimRelative.y - 1 - y_index; // Flip vertically TODO: variable for this

		return Vector2<int>(x_index, y_index);
	}

	int TextureAtlas::GetIndex(const Vector2<int>& vec) const
	{
		return vec.x + (m_AtlasDimRelative.y - 1 - vec.y) * m_AtlasDimRelative.x;
	}

	const std::shared_ptr<Texture> TextureAtlas::GetAtlas() const
	{
		return m_Atlas;
	}

	Vector2<int> TextureAtlas::GetSpriteSize() const
	{
		return m_SpriteSize;
	}

	TextureAtlas::TextureAtlas(std::shared_ptr<Texture> atlas, const Vector2<int>& sprite_size)
		: m_Atlas(atlas), m_SpriteSize(sprite_size)
	{
		m_Construct();
	}

	TextureAtlas::TextureAtlas(const Texture& atlas, const Vector2<int>& sprite_size)
		: m_Atlas(std::make_shared<Texture>(atlas)), m_SpriteSize(sprite_size)
	{
		m_Construct();
	}
	TextureAtlas::TextureAtlas(const std::string& atlas_path, const Vector2<int>& sprite_size)
		: m_Atlas(std::make_shared<Texture>(atlas_path)), m_SpriteSize(sprite_size)
	{
		m_Construct();
	}

	TextureAtlas::Index::Index(int index, TextureAtlas* atlas)
		: val(index)
	{
		vec = atlas->GetVectorIndex(val);
	}

	TextureAtlas::Index::Index(const Vector2<int>& index, TextureAtlas* atlas)
	{
		val = atlas->GetIndex(index);
	}

	Vector2<int> TextureAtlas::Index::GetVector() const
	{
		return vec;
	}

	int TextureAtlas::Index::GetIndex() const
	{
		return val;
	}
}
