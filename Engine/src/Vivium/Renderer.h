#pragma once

#include "Core.h"
#include "Logger.h"
#include "Framebuffer.h"
#include "Color.h"

#define __VIVIUM_DEBUG_PHYSICS_SCENE 0xffff

namespace Vivium {
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
	struct Camera;
	class Text;
	class Button;
	class TextInput;
	class Animation;
	struct Quad;
	class Texture;
	class Slider;
	class Application;
	class Sprite;

	class VIVIUM_API Renderer {
	private:
		static std::vector<uint8_t> m_AvailableSlots; // Available slots for texture TODO: get max texture slots from opengl
		static std::map<int, Framebuffer*> m_Framebuffers;
		static Shader* m_FramebufferShader;

		static Shader* m_CircleShader;

		static void m_Terminate();
		static void m_Init();

		static void m_ResizeFramebuffers(int width, int height);

		static void m_BeginScene(int id);

	public:
		static std::shared_ptr<Camera> camera; // Camera currently being used on all Renderable objects

		static constexpr uint8_t INVALID_TEXTURE_SLOT = 0xff; // Constant to represent an invalid texture slot
		static constexpr uint8_t MAX_TEXTURE_SLOT = 0xf; // TODO: get max texture slots from opengl

		static constexpr int PHYSICS_DEBUG_SCENE = 0x4C2A5994; // NOTE: Just some random number I made up
		static const std::vector<int> RESERVED_SCENE_IDS;

		// Gives a free texture slot
		static uint8_t GetTextureSlot();
		// Makes a texture slot available again
		static void FreeTextureSlot(uint8_t slot);

		static void BeginScene(int id = 0);
		static void EndScene();
		static Framebuffer* GetScene(int id = 0);

		// Draws scenes in order of increasing id
		static void DrawScenes();
		// Draw specific scene
		static void DrawScene(int id);

		// Uses glDrawArrays to draw vertex buffer to screen, count is the number of vertices
		static void Submit(const VertexBuffer* vb, Shader* shader, const std::size_t& count);
		static void Submit(const VertexBuffer* vb, Shader* shader, const Texture* texture, const std::size_t& count);
		static void Submit(const VertexBuffer* vb, const IndexBuffer* ib, Shader* shader);
		static void Submit(const VertexBuffer* vb, const IndexBuffer* ib, Shader* shader, const Texture* texture, uint8_t slot = 0);
		static void Submit(const VertexBuffer* vb, const IndexBuffer* ib, Shader* shader, const Framebuffer* fb, uint8_t slot = 0);
		static void Submit(const Quad* quad, Shader* shader, const Framebuffer* fb, uint8_t slot = 0);
		static void Submit(const Quad* quad, Shader* shader);
		static void Submit(const Quad* quad, Shader* shader, const Texture* texture);
		static void Submit(const Quad* quad, Shader* shader, const Texture* texture, uint8_t slot);
		static void Submit(const Text* text);
		static void Submit(Button* btn);
		static void Submit(Animation* animation);
		static void Submit(TextInput* text_input);
		static void Submit(Slider* slider);
		static void Submit(Sprite* sprite);

		static void DrawCircle(const Vector2<float>& center, float radius, const RGBColor& color);

		friend Application;
	};
}
