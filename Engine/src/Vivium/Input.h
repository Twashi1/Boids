#pragma once

#include "Core.h"
#include "Renderer.h"
#include "Application.h"

// TODO needs an update and cleanup

namespace Vivium {
	class VIVIUM_API Input {
	public:
		enum class VIVIUM_API State : uint8_t {
			NONE = 0,    // If last action was RELEASE, and current action is RELEASE, then NONE
			PRESS = 1,   // If last action was RELEASE, and current action is PRESS, then PRESS
			RELEASE = 2, // If last action was PRESS/HOLD, and current action is RELEASE, then RELEASE
			HOLD = 4     // If last action was PRESS, and current action is PRESS, then HOLD
		};

		enum class VIVIUM_API Mod : int {
			NONE = 0,
			SHIFT = GLFW_MOD_SHIFT,
			CTRL = GLFW_MOD_CONTROL,
			ALT = GLFW_MOD_ALT,
			SUPER = GLFW_MOD_SUPER,
			CAPS_LOCK = GLFW_MOD_CAPS_LOCK,
			NUM_LOCK = GLFW_MOD_NUM_LOCK
		};

		struct VIVIUM_API KeypressLog {
			int key;
			int mods;

			KeypressLog(int key, int mods);
			KeypressLog(const KeypressLog& other);
		};

	private:
		static int m_GetKeyState(int key);		// Return Input::State for given GLFW key code
		static int m_GetMouseState(int mouse);  // Return Input::State for given GLFW btn code

		// Listener object to track last action, current state of a certain key/button, and time button has been held down for
		struct Listener {
			int last_action;	// Last action of button (GLFW_RELEASE/GLFW_PRESSED)
			Input::State state; // Current state of the button
			float time_held;	// Counts seconds key/button has been held down for

			Listener();
		};

		static std::vector<unsigned int> last_chars;	// Stores a log of all (printable) key presses that occured before the last Update
		static std::vector<unsigned int> current_chars; // Stores a log of all (printable) key presses that have occured after the last Update
		static std::vector<KeypressLog> last_keypresses;    // Stores a log of all key presses that occured before the last Update
		static std::vector<KeypressLog> current_keypresses;	// Stores a log of all key presses that have occured after the last Update

		// Callback function for key presses as characters
		static void m_CharCallback(GLFWwindow* window, unsigned int codepoint);
		static void m_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void m_ScrollCallback(GLFWwindow* window, double xoff, double yoff);

		static std::unordered_map<int, Listener> key_listeners;   // Maps a GLFW key code to its respective Listener object
		static std::unordered_map<int, Listener> mouse_listeners; // Maps a GLFW btn code to its respective Listener object

		// Updates a listener's state based on the GLFW action code and time since last update (GLFW_RELEASE/GLFW_PRESSED)
		static void m_UpdateListener(Listener& listener, int current_action, float dt);
		
		static void m_ResetScroll();

		static void m_Init();

		static Timer m_Timer;

		static int m_CurrentMods;
		static float m_LastYScroll; // Last update scroll distance
		static float m_ScrollSensitivity;

	public:
		static void AddKeyListener(int key);
		static void AddMouseListener(int btn);

		static const std::vector<KeypressLog>& GetLastKeypresses();
		static const std::vector<unsigned int>& GetLastChars();

		static int GetMods();
		static bool CheckMod(const Input::Mod& mod);
		static bool CheckMod(const int& mods);

		static float GetScrollSensitivity();
		static void SetScrollSensitivity(float sens);
		static float GetYScroll();

		static bool IsKeyDown(int key);
		static bool IsKeyDown(const Input::State& state);
		static bool IsKeyUp(int key);
		static bool IsKeyUp(const Input::State& state);

		static Input::State GetKeyState(int key);
		static Input::State GetMouseState(int button);
		// Get position of cursor in screen coordinates (0, 0) -> (width, height)
		static Vector2<float> GetCursorPos();
		// Get position of cursor within range (0.0, 0.0) -> (1.0, 1.0) in UV coordinates
		static Vector2<float> GetCursorUVPos();

		// Update all listener objects, keypress logs, etc.
		static void Update();

		friend Application;
	};
	
	VIVIUM_API int operator|(const Input::Mod& a, const Input::Mod& b);
	VIVIUM_API int operator|(int a, const Input::Mod& b);
	VIVIUM_API int operator|(const Input::Mod& a, int b);
	VIVIUM_API int operator&(const Input::Mod& a, const Input::Mod& b);
	VIVIUM_API int operator&(int a, const Input::Mod& b);
	VIVIUM_API int operator&(const Input::Mod& a, int b);

	// TODO add formatter to replace this
	VIVIUM_API std::string to_string(const Input::Mod& mod);
}

namespace std {
	template <>
	struct formatter<Vivium::Input::State> : formatter<string> {
		auto format(Vivium::Input::State state, format_context& ctx) {
			std::string state_string;

			switch (state) {
			case Vivium::Input::State::NONE: state_string = "None"; break;
			case Vivium::Input::State::PRESS: state_string = "Press"; break;
			case Vivium::Input::State::HOLD: state_string = "Held"; break;
			case Vivium::Input::State::RELEASE: state_string = "Released"; break;
			}

			return formatter<string>::format(
				state_string, ctx);
		}
	};
}
