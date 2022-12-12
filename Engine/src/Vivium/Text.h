#pragma once

#include "Renderer.h"
#include "Font.h"
#include "Shader.h"
#include "Application.h"
#include "Batch.h"

namespace Vivium {
	class VIVIUM_API Text : Batchable {
	private:
		static std::shared_ptr<Shader> m_DefaultShader;
		static std::shared_ptr<Font> m_DefaultFont;

		void m_Construct(const std::string& input_str);

		static void m_Init();

		float m_GetWidth(const std::string& str) const;

		std::shared_ptr<Texture> m_FontTexture;
		std::shared_ptr<TextureAtlas> m_FontTextureAtlas;

	public:
		enum class Alignment : uint8_t {
			NONE, // TODO: add some validation
			LEFT,
			CENTER,
			RIGHT // TODO: implement
		};

		Alignment alignment;
		std::vector<std::string> strings;				// Strings to render, split by new lines
		std::shared_ptr<Vector2<float>> pos;			// Position to render text
		std::shared_ptr<Font> font;						// Pointer to font
		std::shared_ptr<Shader> shader;					// Shader for text
		float scale;									// Scale for text

		static std::shared_ptr<Font> GetDefaultFont();

		// TODO: might need fixes
		void SetPos(const Vector2<float>& pos);
		Vector2<float> GetPos() const;

		void FixToWidth(float max_width);

		float GetWidth() const;
		float GetHeight() const;

		std::string GetText() const;
		void SetText(const std::string& str);

		const std::shared_ptr<TextureAtlas> GetAtlas() const;
		const std::shared_ptr<Texture> GetTexture() const;

		void Render() const;
		void Submit(Batch* batch) const override;

		Text();
		Text(const Text& other);
		Text(const std::string& text, const Vector2<float>& pos, const Alignment& alignment = Alignment::LEFT, float scale = 1.0f);
		Text(const std::string& text, const Vector2<float>& pos, std::shared_ptr<Font> font, const Alignment& alignment = Alignment::LEFT, float scale = 1.0f);
		Text(const std::string& text, const Vector2<float>& pos, std::shared_ptr<Shader> shader, const Alignment& alignment = Alignment::LEFT, float scale = 1.0f);
		Text(const std::string& text, const Vector2<float>& pos, std::shared_ptr<Font> font, std::shared_ptr<Shader> shader, const Alignment& alignment = Alignment::LEFT, float scale = 1.0f);

		~Text();

		friend Application;
	};
}