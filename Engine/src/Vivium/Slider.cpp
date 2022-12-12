#include "Slider.h"

namespace Vivium {
	std::shared_ptr<Shader> Slider::m_DefaultBarShader = nullptr;
	std::shared_ptr<Shader> Slider::m_DefaultSliderShader = nullptr;

	void Slider::m_UpdateValue()
	{
		m_Value = (m_SliderQuad->GetX() - m_BarQuad->Left()) / m_BarQuad->GetWidth();
	}

	void Slider::m_Construct()
	{
		// Calculate initial value from quad position
		float initial = (m_SliderQuad->GetX() - m_BarQuad->Left()) / m_BarQuad->GetWidth();
		// Clamp it from 0 to 1 and set m_Value
		SetValue(std::min(std::max(initial, 0.0f), 1.0f));
	}

	void Slider::m_Init()
	{
		m_DefaultBarShader = std::make_shared<Shader>("static_texture_vertex", "slider_frag");
		m_DefaultBarShader->SetUniform3f("u_BarColor", 0.3, 0.3, 0.3);
		m_DefaultBarShader->SetUniform3f("u_ShadedColor", 0.2, 0.2, 0.2);

		m_DefaultSliderShader = std::make_shared<Shader>("static_texture_vertex", "color_frag");
		m_DefaultSliderShader->SetUniform3f("u_Color", 0.2, 0.2, 0.2);
	}

	float Slider::GetValue() const
	{
		return m_Value;
	}

	float Slider::GetValue(float scale) const
	{
		return m_Value * scale;
	}

	float Slider::GetValue(float min, float max) const
	{
		return (m_Value * (max - min)) + min;
	}

	void Slider::SetValue(float value)
	{
		// TODO update slider subroutine?
		m_Value = value;
		m_SliderQuad->SetX(m_BarQuad->Left() + m_BarQuad->GetWidth() * m_Value);
	}

	void Slider::SetValue(float value, float scale)
	{
		m_Value = value / scale;
		m_SliderQuad->SetX(m_BarQuad->Left() + m_BarQuad->GetWidth() * m_Value);
	}

	void Slider::SetValue(float value, float min, float max)
	{
		m_Value = (value - min) / (max - min);
		m_SliderQuad->SetX(m_BarQuad->Left() + m_BarQuad->GetWidth() * m_Value);
	}

	Vector2<float> Slider::GetPos() const
	{
		return m_BarQuad->GetCenter();
	}

	void Slider::SetPos(const Vector2<float>& new_pos)
	{
		m_BarQuad->SetCenter(new_pos);
		// TODO: two UpdateVB calls
		m_SliderQuad->SetX(m_BarQuad->Left() + m_BarQuad->GetWidth() * m_Value);
		m_SliderQuad->SetY(m_BarQuad->GetY());
	}

	void Slider::Update()
	{
		Vector2<float> cursor_pos = Input::GetCursorPos();

		// If our cursor is above the slider
		if (isMovingSlider || m_BarQuad->Contains(cursor_pos) || m_SliderQuad->Contains(cursor_pos)) {
			isMovingSlider = true;

			// If pressing or holding slider
			Input::State lmb_state = Input::GetMouseState(GLFW_MOUSE_BUTTON_1);

			if (lmb_state == Input::State::PRESS || lmb_state == Input::State::HOLD) {
				float new_pos = std::min(std::max(cursor_pos.x, m_BarQuad->Left()), m_BarQuad->Right());

				// Update position
				m_SliderQuad->SetX(new_pos);
				// Update value based on new position
				m_UpdateValue();
				// Run callback
				if (callback != nullptr) callback(this, m_UserParams);
			}
			else {
				isMovingSlider = false;
			}
		}

		bar_shader->Bind();
		bar_shader->SetUniform1f("u_Value", m_Value);
	}

	Slider::Slider(std::shared_ptr<Quad> bar_quad, std::shared_ptr<Quad> slider_quad, CallbackFunc_t callback, void* userParams)
		: m_Value(0.0f), m_BarQuad(bar_quad), m_SliderQuad(slider_quad), callback(callback), bar_shader(m_DefaultBarShader), slider_shader(m_DefaultSliderShader), m_UserParams(userParams)
	{
		m_Construct();
	}

	Slider::Slider(std::shared_ptr<Quad> bar_quad, std::shared_ptr<Quad> slider_quad, std::shared_ptr<Shader> bar_shader, std::shared_ptr<Shader> slider_shader, CallbackFunc_t callback, void* userParams)
		: m_Value(0.0f), m_BarQuad(bar_quad), m_SliderQuad(slider_quad), callback(callback), bar_shader(bar_shader), slider_shader(slider_shader), m_UserParams(userParams)
	{
		m_Construct();
	}

	Slider::Slider(std::shared_ptr<Quad> bar_quad, std::shared_ptr<Quad> slider_quad, std::shared_ptr<Shader> bar_shader, std::shared_ptr<Shader> slider_shader, std::shared_ptr<Texture> bar_texture, std::shared_ptr<Texture> slider_texture, CallbackFunc_t callback, void* userParams)
		: m_Value(0.0f), m_BarQuad(bar_quad), m_SliderQuad(slider_quad), callback(callback), bar_shader(bar_shader), slider_shader(slider_shader), bar_texture(bar_texture), slider_texture(slider_texture), m_UserParams(userParams)
	{
		m_Construct();
	}

	Slider::~Slider()
	{
	}
}