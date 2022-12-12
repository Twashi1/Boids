#pragma once

#include "Logger.h"
#include "Quad.h"
#include "Shader.h"
#include "Texture.h"
#include "Application.h"

// TODO: unused
// TODO: bad support for texture atlases

namespace Vivium {
	class VIVIUM_API Sprite {
	private:
		static std::shared_ptr<Shader> m_DefaultStaticTextureShader;
		static std::shared_ptr<Shader> m_DefaultWorldTextureShader;

		static void m_Init();

	public:
		std::shared_ptr<Quad> quad;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture> texture = nullptr;
	
		Sprite(std::shared_ptr<Quad> quad, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture = nullptr);
		Sprite(std::shared_ptr<Quad> quad, std::shared_ptr<Texture> texture, bool isStatic = false);
		Sprite(const Vector2<float>& pos, const Vector2<float>& size, const std::string& sprite_name, bool isStatic = false);
	
		friend Application;
	};
}
