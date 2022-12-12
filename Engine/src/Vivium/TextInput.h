#pragma once

#include "Logger.h"
#include "Input.h"
#include "Text.h"
#include "Font.h"
#include "Animation.h"
#include "Application.h"

namespace Vivium {
	// TODO: rename "m_HintText" to "m_HintText"
	class VIVIUM_API TextInput {
	public:
		typedef void (*CallbackFunc_t)(TextInput*, void*); // Shorthand for a callback function pointer (takes the text input that was entered as a paramater)

		static std::shared_ptr<Animation> m_TypingBar; // Animation for the bar that shows where your cursor currently is
		static RGBColor m_DefaultEmptyColor; // Default color for text that displays when input is empty
		static RGBColor m_DefaultTypedColor; // Default color for text that user has typed in
		static std::string m_DefaultHintText; // Default text that displays when box is empty
		static std::shared_ptr<Font> m_DefaultFont;
		static std::shared_ptr<Shader> m_DefaultBgShader;
		static std::shared_ptr<Shader> m_TextShader;
		static std::shared_ptr<Shader> m_TypingBarShader;

		static bool m_TypingOnAnyInput;

		static GLFWcursor* m_Cursor;

		static constexpr float m_DefaultScale = 0.25f; // Standard scaling factor for text (1/4 size)

		// Constructs the objects
		void m_Construct();

		// TODO implement stall time
		float m_StallTime = 0.0; // After typing a letter, the typing bar stays visible for a small amount of time extra, this tracks how long it has to stall for
		Timer m_Timer;

		bool m_IsTyping = false;
		int m_TypingIndex = 0; // Current index of where we're typing in the string

		std::size_t m_CharLimit = 20;

		RGBColor m_HintColor; // Color for "m_HintText"
		RGBColor m_TypedColor; // Color for text user typed in

		std::shared_ptr<Text> m_Text = nullptr;

		// TODO: ALLOW USER TO SET THIS
		std::string m_HintText;	// Text that displays when textinput is empty

		// Updates the text based on user input
		void m_UpdateText();

		static void m_Init(std::shared_ptr<TextureAtlas> engine_icons);

		static void m_ResetTypingStatus();

		CallbackFunc_t callback;
		void* m_UserParams;

		int m_Offset; // Offset of text from the left side of the text input quad

	public:
		std::string typed_text = ""; // Text that user has typed/is typing in

		std::shared_ptr<Texture> bg_texture = nullptr;
		std::shared_ptr<Shader> bg_shader;

		std::shared_ptr<Quad> quad;

		void SetPos(const Vector2<float>& new_pos);
		Vector2<float> GetPos() const;

		std::shared_ptr<Animation> GetTypingBar();
		bool GetIsTyping();
		std::shared_ptr<Text> GetText();

		void SetUserParams(void* userParams);
		void SetHintText(const std::string& text);

		void SetCharLimit(const std::size_t& char_limit);

		TextInput(const Quad& quad, CallbackFunc_t callback = nullptr, void* userParams = nullptr, int offset = 0);
		TextInput(const Quad& quad, CallbackFunc_t callback, std::shared_ptr<Shader> bg_shader, void* userParams = nullptr, int offset = 0);
		TextInput(const Quad& quad, CallbackFunc_t callback, std::shared_ptr<Shader> bg_shader, std::shared_ptr<Texture> bg_texture, void* userParams = nullptr, int offset = 0);
		TextInput(const Quad& quad, CallbackFunc_t callback, const RGBColor& typed_color, const RGBColor& empty_color, void* userParams = nullptr, int offset = 0);
		TextInput(const Quad& quad, CallbackFunc_t callback, const RGBColor& typed_color, const RGBColor& empty_color, std::shared_ptr<Shader> bg_shader, void* userParams = nullptr, int offset = 0);
		TextInput(const Quad& quad, CallbackFunc_t callback, const RGBColor& typed_color, const RGBColor& empty_color, std::shared_ptr<Shader> bg_shader, std::shared_ptr<Texture> bg_texture, void* userParams = nullptr, int offset = 0);
		~TextInput();

		void CheckClicked(const Vector2<float>& cursor_pos, const Input::State& lmb_state);
		void Update();

		friend Application;
	};
}
