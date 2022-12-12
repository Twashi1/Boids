#include "Button.h"

namespace Vivium {
	std::shared_ptr<Shader> Button::m_DefaultIdleShader = nullptr;
	std::shared_ptr<Shader> Button::m_DefaultPressedShader = nullptr;
	std::shared_ptr<Font> Button::m_DefaultFont = nullptr;

	void Button::m_Construct()
	{
		// Construct text object
		Vector2<float> pos = quad->GetCenter();
		text = std::make_shared<Text>(idle_text, pos, Text::Alignment::CENTER, Button::m_DefaultScale);
	}

	void Button::m_Init()
	{
		VIVIUM_SCOPE;

		// Check if class has been initialised already (the standard shaders are default until initialised)
		if (m_DefaultIdleShader != nullptr) {
			LogWarn("Attempting to reinitialise Button class");
		}

		// Construct default shaders
		m_DefaultIdleShader = std::make_shared<Shader>("static_vertex", "transparency_frag");
		m_DefaultIdleShader->SetUniform4f("u_Color", 0.3, 0.3, 0.3, Button::m_DefaultAlpha);

		m_DefaultPressedShader = std::make_shared<Shader>("static_vertex", "transparency_frag");
		m_DefaultPressedShader->SetUniform4f("u_Color", 0.2, 0.2, 0.2, Button::m_DefaultAlpha);

		// Construct default font
		m_DefaultFont = Text::GetDefaultFont();
	}

	void Button::SetPos(const Vector2<float>& pos)
	{
		quad->SetCenter(pos);
		m_UpdatePos();
	}

	Vector2<float> Button::GetPos() const
	{
		return quad->GetCenter();
	}

	Button::Button(const Quad& quad, CallbackFunc_t callback, const std::string& idle_text, const std::string& pressed_text, std::shared_ptr<Shader> idle_shader, std::shared_ptr<Shader> pressed_shader, std::shared_ptr<Texture> idle_texture, std::shared_ptr<Texture> pressed_texture, void* userParams)
		:
		callback(callback),
		idle_text(idle_text), pressed_text(pressed_text),
		idle_shader(idle_shader), pressed_shader(pressed_shader),
		idle_texture(idle_texture), pressed_texture(pressed_texture),
		m_UserParams(userParams)
	{
		this->quad = std::make_shared<Quad>(quad);
		m_Construct();
	}

	Button::Button(const Quad& quad, CallbackFunc_t callback, const std::string& idle_text, const std::string& pressed_text, std::shared_ptr<Shader> idle_shader, std::shared_ptr<Shader> pressed_shader, void* userParams)
		:
		callback(callback),
		idle_text(idle_text), pressed_text(pressed_text),
		idle_shader(idle_shader), pressed_shader(pressed_shader),
		m_UserParams(userParams)
	{
		this->quad = std::make_shared<Quad>(quad);
		m_Construct();
	}

	Button::Button(const Quad& quad, CallbackFunc_t callback, const std::string& idle_text, const std::string& pressed_text, void* userParams)
		:
		callback(callback),
		idle_text(idle_text), pressed_text(pressed_text),
		idle_shader(m_DefaultIdleShader), pressed_shader(m_DefaultPressedShader),
		m_UserParams(userParams)
	{
		this->quad = std::make_shared<Quad>(quad);
		m_Construct();
	}

	Button::Button(const Quad& quad, CallbackFunc_t callback, const std::string& text, void* userParams)
		:
		callback(callback),
		idle_text(text), pressed_text(text),
		idle_shader(m_DefaultIdleShader), pressed_shader(m_DefaultPressedShader),
		m_UserParams(userParams)
	{
		this->quad = std::make_shared<Quad>(quad);
		m_Construct();
	}

	Button::~Button()
	{
	}

	void Button::SetUserParams(void* userParams) { m_UserParams = userParams; }

	void Button::Update()
	{
		VIVIUM_SCOPE;

		Vector2<float> cursor_pos = Input::GetCursorPos();
		Input::State lmb_state = Input::GetMouseState(GLFW_MOUSE_BUTTON_1);

		m_UpdatePos();
		if (lmb_state == Input::State::RELEASE) {
			CheckClicked(cursor_pos);
		}
		else if (lmb_state == Input::State::PRESS || lmb_state == Input::State::HOLD) {
			CheckPressed(cursor_pos);
		}
	}

	void Button::CheckClicked(const Vector2<float>& cursor_pos)
	{
		VIVIUM_SCOPE;

		// Reset back to default state, regardless if we specifically were the button pressed
		// This is so if you pressed on a button, then move your cursor away and release, the button
		// will still reset back to its default state (but doesn't register as us being clicked)

		// We're not being pressed anymore
		isPressed = false;

		// Update text
		text->SetText(isPressed ? pressed_text : idle_text);

		// If the cursor is within the bounds of the button, run callback function
		if (quad->Contains(cursor_pos)) {
			// Execute callback function
			callback(this, m_UserParams);

			// Update position
			m_UpdatePos();
		}
	}

	void Button::CheckPressed(const Vector2<float>& cursor_pos)
	{
		VIVIUM_SCOPE;

		// If the cursor is within the bounds of the button
		if (quad->Contains(cursor_pos)) {
			// We're being pressed
			isPressed = true;

			// Update text
			text->SetText(isPressed ? pressed_text : idle_text);

			// Update position
			m_UpdatePos();
		}
	}

	std::shared_ptr<Shader> Button::CurrentShader()
	{
		return isPressed ? pressed_shader : idle_shader;
	}

	std::shared_ptr<Texture> Button::CurrentTexture()
	{
		return isPressed ? pressed_texture : idle_texture;
	}

	float Button::GetAlpha() const
	{
		return m_DefaultAlpha;
	}

	const std::string& Button::CurrentText() const
	{
		return isPressed ? pressed_text : idle_text;
	}

	void Button::m_UpdatePos()
	{
		*text->pos = quad->GetCenter();
	}
}