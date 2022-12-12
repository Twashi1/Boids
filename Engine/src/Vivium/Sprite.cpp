#include "Sprite.h"

namespace Vivium {
	std::shared_ptr<Shader> Sprite::m_DefaultStaticTextureShader = nullptr;
	std::shared_ptr<Shader> Sprite::m_DefaultWorldTextureShader = nullptr;

	void Sprite::m_Init()
	{
		m_DefaultStaticTextureShader = std::make_shared<Shader>("static_texture_vertex", "texture_frag");
		m_DefaultWorldTextureShader = std::make_shared<Shader>("texture_vertex", "texture_frag");
	}

	Sprite::Sprite(std::shared_ptr<Quad> quad, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture)
		: quad(quad), shader(shader), texture(texture)
	{
	}

	Sprite::Sprite(std::shared_ptr<Quad> quad, std::shared_ptr<Texture> texture, bool isStatic)
		: quad(quad), texture(texture)
	{
		if (isStatic) {
			shader = m_DefaultStaticTextureShader;
		}
		else {
			shader = m_DefaultWorldTextureShader;
		}
	}

	Sprite::Sprite(const Vector2<float>& pos, const Vector2<float>& size, const std::string& sprite_name, bool isStatic)
	{
		if (isStatic) {
			shader = m_DefaultStaticTextureShader;
		}
		else {
			shader = m_DefaultWorldTextureShader;
		}

		quad = std::make_shared<Quad>(pos, size);
		texture = std::make_shared<Texture>(sprite_name);
	}
}
