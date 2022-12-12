#include "Input.h"

namespace Vivium {
	int Input::m_CurrentMods = (int)Input::Mod::NONE;
	float Input::m_LastYScroll = 0.0f;
	float Input::m_ScrollSensitivity = 10.0f;

	std::unordered_map<int, Input::Listener> Input::key_listeners{};
	std::unordered_map<int, Input::Listener> Input::mouse_listeners{};

	std::vector<unsigned int> Input::last_chars{};
	std::vector<unsigned int> Input::current_chars{};
	std::vector<Input::KeypressLog> Input::last_keypresses{};
	std::vector<Input::KeypressLog> Input::current_keypresses{};

	Timer Input::m_Timer = Timer();

	int Input::m_GetKeyState(int key)
	{
		return glfwGetKey(Application::window, key);
	}

	int Input::m_GetMouseState(int mouse)
	{
		return glfwGetMouseButton(Application::window, mouse);
	}

	void Input::m_CharCallback(GLFWwindow* window, unsigned int codepoint)
	{
		current_chars.push_back(codepoint);
	}

	void Input::m_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS) {
			current_keypresses.push_back(KeypressLog{ key, mods });
		}
	}

	void Input::m_ScrollCallback(GLFWwindow* window, double xoff, double yoff)
	{
		m_LastYScroll = yoff;
	}

	void Input::m_UpdateListener(Listener& listener, int current_action, float dt)
	{
		if (listener.state == State::HOLD) listener.time_held += dt;
		else listener.time_held = 0.0f;

		if (listener.last_action == GLFW_RELEASE) {
			if (current_action == GLFW_PRESS)		 listener.state = State::PRESS;
			else if (current_action == GLFW_RELEASE) listener.state = State::NONE;
		}
		else if (listener.last_action == GLFW_PRESS) {
			if (current_action == GLFW_PRESS)		 listener.state = State::HOLD;
			else if (current_action == GLFW_RELEASE) listener.state = State::RELEASE;
		}

		listener.last_action = current_action;
	}

	void Input::m_ResetScroll()
	{
		m_LastYScroll = 0.0f;
	}

	void Input::m_Init()
	{
		for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; i++) {
			key_listeners[i] = Listener();
		}

		for (int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST; i++) {
			mouse_listeners[i] = Listener();
		}

		glfwSetCharCallback(Application::window, m_CharCallback);
		glfwSetKeyCallback(Application::window, m_KeyCallback);
		glfwSetScrollCallback(Application::window, m_ScrollCallback);
	}

	void Input::AddKeyListener(int key)
	{
		// If key is not already in our listeners, add
		if (!key_listeners.contains(key)) key_listeners[key] = Listener();
	}

	void Input::AddMouseListener(int btn)
	{
		// If btn is not already in our listeners, add
		if (!mouse_listeners.contains(btn)) mouse_listeners[btn] = Listener();
	}

	const std::vector<Input::KeypressLog>& Input::GetLastKeypresses()
	{
		return last_keypresses;
	}

	const std::vector<unsigned int>& Input::GetLastChars()
	{
		return last_chars;
	}

	int Input::GetMods()
	{
		return m_CurrentMods;
	}

	bool Input::CheckMod(const Input::Mod& mod)
	{
		return m_CurrentMods & mod;
	}

	float Input::GetScrollSensitivity() { return m_ScrollSensitivity; }
	void Input::SetScrollSensitivity(float sens) { m_ScrollSensitivity = sens; }

	float Input::GetYScroll()
	{
		return m_LastYScroll * m_ScrollSensitivity;
	}

	bool Input::CheckMod(const int& mods)
	{
		return m_CurrentMods & mods;
	}

	bool Input::IsKeyDown(int key) { auto state = GetKeyState(key); return state == State::PRESS || state == State::HOLD; }
	bool Input::IsKeyDown(const Input::State& state) { return state == State::PRESS || state == State::HOLD; }
	bool Input::IsKeyUp(int key) { auto state = GetKeyState(key); return state == State::RELEASE || state == State::NONE; }
	bool Input::IsKeyUp(const Input::State& state) { return state == State::RELEASE || state == State::NONE; }

	Input::State Input::GetKeyState(int key)
	{
		// Get iterator to key state
		auto it = key_listeners.find(key);

		// If iterator exists - key exists in key_listeners
		if (it != key_listeners.end()) {
			// Return key state
			return it->second.state;
		}

		LogWarn("Couldn't find key {}", key);

		return State::NONE;
	}

	Input::State Input::GetMouseState(int button)
	{
		// Get iterator to button state
		auto it = mouse_listeners.find(button);

		// If iterator exists - button exists in mouse_listeners
		if (it != mouse_listeners.end()) {
			// Return button state
			return it->second.state;
		}

		LogWarn("Couldn't find button {}", button);

		return State::NONE;
	}

	Vector2<float> Input::GetCursorPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(Application::window, &xpos, &ypos);

		return Vector2<float>(xpos, Application::height - ypos);
	}

	Vector2<float> Input::GetCursorUVPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(Application::window, &xpos, &ypos);

		float width = (float)Application::width;
		float height = (float)Application::height;

		// Divide by width and height, and invert height to convert to uv coordinates
		return Vector2<float>(xpos / (float)width, (height - ypos) / (float)height);
	}

	void Input::Update()
	{
		float elapsed = m_Timer.GetElapsed();

		// Update each key listener
		for (auto& [key, listener] : key_listeners) {
			int current_action = m_GetKeyState(key);

			m_UpdateListener(listener, current_action, elapsed);
		}

		// Update each mouse listener
		for (auto& [btn, listener] : mouse_listeners) {
			int current_action = m_GetMouseState(btn);

			m_UpdateListener(listener, current_action, elapsed);
		}

		// Update keypress logs
		last_keypresses = current_keypresses;
		current_keypresses.clear();

		last_chars = current_chars;
		current_chars.clear();
	}

	Input::Listener::Listener()
		: last_action(GLFW_RELEASE), state(Input::State::NONE), time_held(0.0f)
	{}

	VIVIUM_API int operator|(const Input::Mod& a, const Input::Mod& b)
	{
		return (int)a | (int)b;
	}

	VIVIUM_API int operator|(int a, const Input::Mod& b)
	{
		return a | (int)b;
	}

	VIVIUM_API int operator|(const Input::Mod& a, int b)
	{
		return (int)a | b;
	}

	VIVIUM_API int operator&(const Input::Mod& a, const Input::Mod& b)
	{
		return (int)a & (int)b;
	}

	VIVIUM_API int operator&(int a, const Input::Mod& b)
	{
		return a & (int)b;
	}

	VIVIUM_API int operator&(const Input::Mod& a, int b)
	{
		return (int)a & b;
	}

	VIVIUM_API std::string to_string(const Input::State& state)
	{
		switch (state) {
		case Input::State::HOLD: return "Hold";
		case Input::State::NONE: return "None";
		case Input::State::PRESS: return "Press";
		case Input::State::RELEASE: return "Release";
		default: LogWarn("Invalid input state {}", (uint8_t)state); return "Invalid";
		}
	}

	VIVIUM_API std::string to_string(const Input::Mod& mod)
	{
		switch (mod) {
		case Input::Mod::ALT: return "Alt";
		case Input::Mod::SHIFT: return "Shift";
		case Input::Mod::NONE: return "None";
		case Input::Mod::CAPS_LOCK: return "Caps lock";
		case Input::Mod::NUM_LOCK: return "Num lock";
		case Input::Mod::SUPER: return "Super";
		case Input::Mod::CTRL: return "Ctrl";
		default: LogWarn("Invalid mod {}", (int)mod); return "Invalid";
		}
	}

	Input::KeypressLog::KeypressLog(int key, int mods)
		: key(key), mods(mods)
	{}

	Input::KeypressLog::KeypressLog(const KeypressLog& other)
		: key(other.key), mods(other.mods)
	{}
}