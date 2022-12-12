#pragma once

#include "Logger.h"
#include "Quad.h"
#include "Shader.h"
#include "Text.h"
#include "Input.h"
#include "Application.h"

namespace Vivium {
	class VIVIUM_API Slider {
	public:
		static std::shared_ptr<Shader> m_DefaultBarShader;
		static std::shared_ptr<Shader> m_DefaultSliderShader;

		float m_Value; // Slider value from 0 -> 1

		void m_UpdateValue();
		void m_Construct();

		std::shared_ptr<Quad> m_BarQuad;
		std::shared_ptr<Quad> m_SliderQuad;

		bool isMovingSlider = false; // Currently holding down the slider

		void* m_UserParams = nullptr;

		static void m_Init();

	public:
		typedef void (*CallbackFunc_t)(Slider*, void*);

		CallbackFunc_t callback;

		std::shared_ptr<Texture> bar_texture = nullptr;
		std::shared_ptr<Shader> bar_shader;

		std::shared_ptr<Texture> slider_texture = nullptr;
		std::shared_ptr<Shader> slider_shader;

		float GetValue() const;
		float GetValue(float scale) const;
		float GetValue(float min, float max) const;

		void SetValue(float value);
		void SetValue(float value, float scale);
		void SetValue(float value, float min, float max);

		Vector2<float> GetPos() const;
		void SetPos(const Vector2<float>& new_pos);

		void SetUserParams(void* userParams) { m_UserParams = userParams; }

		void Update();

		Slider(std::shared_ptr<Quad> bar_quad, std::shared_ptr<Quad> slider_quad, CallbackFunc_t callback, void* userParams = nullptr);
		Slider(std::shared_ptr<Quad> bar_quad, std::shared_ptr<Quad> slider_quad, std::shared_ptr<Shader> bar_shader, std::shared_ptr<Shader> slider_shader, CallbackFunc_t callback, void* userParams = nullptr);
		Slider(std::shared_ptr<Quad> bar_quad, std::shared_ptr<Quad> slider_quad, std::shared_ptr<Shader> bar_shader, std::shared_ptr<Shader> slider_shader, std::shared_ptr<Texture> bar_texture, std::shared_ptr<Texture> slider_texture, CallbackFunc_t callback, void* userParams = nullptr);
		~Slider();

		friend Renderer;
		friend Application;
	};
}
