#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Font.h"
#include "Camera.h"
#include "Text.h"
#include "Animation.h"
#include "Quad.h"
#include "Button.h"
#include "TextInput.h"
#include "Slider.h"
#include "Sprite.h"
#include "Application.h"

namespace Vivium {
	std::vector<uint8_t> Renderer::m_AvailableSlots = std::vector<uint8_t>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });
	std::shared_ptr<Camera> Renderer::camera = nullptr;

	std::map<int, Framebuffer*> Renderer::m_Framebuffers = {};
	Shader* Renderer::m_FramebufferShader = nullptr;
	Shader* Renderer::m_CircleShader = nullptr;

	const std::vector<int> Renderer::RESERVED_SCENE_IDS = {Renderer::PHYSICS_DEBUG_SCENE};

	void Renderer::m_Terminate()
	{
		Framebuffer::Unbind();

		for (auto [id, fb] : m_Framebuffers) {
			delete fb;
		}

		m_Framebuffers = {};

		delete m_FramebufferShader;
		delete m_CircleShader;
	}

	void Renderer::m_Init()
	{
		camera = std::make_shared<Camera>(0.0f, (float)Application::width, 0.0f, (float)Application::height);

		m_FramebufferShader = new Shader("static_texture_vertex", "texture_frag");
		m_CircleShader = new Shader("texture_vertex", "circle_frag");
	}

	void Renderer::m_ResizeFramebuffers(int width, int height)
	{
		VIVIUM_SCOPE;

		for (auto& it : m_Framebuffers) {
			it.second->Resize(width, height);
		}
	}

	void Renderer::m_BeginScene(int id)
	{
		VIVIUM_SCOPE;

		auto it = m_Framebuffers.find(id);

		// If framebuffer already exists
		if (it != m_Framebuffers.end()) {
			it->second->Bind();
		}
		// Framebuffer doesn't already exist, create it
		else {
			Vector2<int> screen_dim = Application::GetScreenDim();
			Framebuffer* new_fb = new Framebuffer(screen_dim.x, screen_dim.y);
			m_Framebuffers.insert({ id, new_fb });
			new_fb->Bind();
		}
	}

	void Renderer::BeginScene(int id)
	{
		VIVIUM_SCOPE;

		// Using a reserved id
		auto id_it = std::find(RESERVED_SCENE_IDS.begin(), RESERVED_SCENE_IDS.end(), id);

		if (id_it == RESERVED_SCENE_IDS.end()) {
			std::size_t index = std::distance(RESERVED_SCENE_IDS.begin(), id_it);
			LogError("Attempted to use reserved scene id: {}", RESERVED_SCENE_IDS[index]);

			return;
		}

		m_BeginScene(id);
	}

	void Renderer::EndScene()
	{
		Framebuffer::Unbind();
	}

	Framebuffer* Renderer::GetScene(int id)
	{
		return m_Framebuffers.at(id);
	}

	void Renderer::DrawScenes()
	{
		VIVIUM_SCOPE;

		Vector2<float> screen_dim = (Vector2<float>)Application::GetScreenDim();

		Quad screen_quad(screen_dim * 0.5f, screen_dim); // TODO: could be static

		for (auto& [id, fb] : m_Framebuffers) {
			Renderer::Submit(&screen_quad, m_FramebufferShader, fb, 0);

			fb->Clear();
		}
	}

	void Renderer::DrawScene(int id)
	{
		VIVIUM_SCOPE;

		auto it = m_Framebuffers.find(id);

		if (it != m_Framebuffers.end()) {
			Vector2<float> screen_dim = (Vector2<float>)Application::GetScreenDim();

			Quad screen_quad(screen_dim * 0.5f, screen_dim);

			Renderer::Submit(&screen_quad, m_FramebufferShader, it->second, 0);
			
			it->second->Clear();
		}
		else {
			LogError("Couldn't find scene with ID {}, ignoring call", id);
		}
	}
	
	uint8_t Renderer::GetTextureSlot()
	{
		VIVIUM_SCOPE;

		if (m_AvailableSlots.size() > 1) {
			// Get front of queue and store it
			uint8_t id = m_AvailableSlots.back();
			// Remove that id from list
			m_AvailableSlots.pop_back();
			// Return that id
			return id;
		}

		LogInfo("Ran out of texture slots (MAX 16 concurrent)");
		return Renderer::INVALID_TEXTURE_SLOT;
	}

	void Renderer::FreeTextureSlot(uint8_t slot)
	{
		if (slot != Renderer::INVALID_TEXTURE_SLOT) {
			m_AvailableSlots.emplace_back(slot);
		}
	}

	void Renderer::Submit(const VertexBuffer* vb, const IndexBuffer* ib, Shader* shader, const Texture* texture, uint8_t slot)
	{
		VIVIUM_SCOPE;

		vb->Bind(); ib->Bind(); shader->Bind(); texture->Bind(slot);
		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());
		shader->SetUniform1i("u_Texture", slot);

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);
	}

	void Renderer::Submit(const VertexBuffer* vb, Shader* shader, const std::size_t& count)
	{
		VIVIUM_SCOPE;

		vb->Bind(); shader->Bind();

		glDrawArrays(GL_TRIANGLES, 0, count);
	}

	void Renderer::Submit(const VertexBuffer* vb, Shader* shader, const Texture* texture, const std::size_t& count)
	{
		VIVIUM_SCOPE;

		uint8_t slot = GetTextureSlot();
		vb->Bind(); shader->Bind(); texture->Bind(slot);
		shader->SetUniform1i("u_Texture", slot);

		glDrawArrays(GL_TRIANGLES, 0, count);

		FreeTextureSlot(slot);
	}

	void Renderer::Submit(const VertexBuffer* vb, const IndexBuffer* ib, Shader* shader)
	{
		VIVIUM_SCOPE;

		vb->Bind(); ib->Bind(); shader->Bind();
		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);
	}

	void Renderer::Submit(const VertexBuffer* vb, const IndexBuffer* ib, Shader* shader, const Framebuffer* fb, uint8_t slot)
	{
		VIVIUM_SCOPE;

		vb->Bind(); ib->Bind(); shader->Bind(); fb->SetSlot(slot);
		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());
		shader->SetUniform1i("u_Texture", slot);

		fb->SetSlot(slot);

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);
	}

	void Renderer::Submit(const Quad* quad, Shader* shader, const Framebuffer* fb, uint8_t slot)
	{
		VIVIUM_SCOPE;

		quad->GetVertexBuffer()->Bind();
		const IndexBuffer* ib = Quad::GetIndexBuffer(); ib->Bind();
		shader->Bind(); fb->SetSlot(slot);

		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());
		shader->SetUniform1i("u_Texture", slot);

		fb->SetSlot(slot);

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);
	}

	void Renderer::Submit(const Quad* quad, Shader* shader)
	{
		VIVIUM_SCOPE;

		quad->GetVertexBuffer()->Bind();
		const IndexBuffer* ib = Quad::GetIndexBuffer(); ib->Bind();
		shader->Bind();
		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);
	}

	void Renderer::Submit(const Quad* quad, Shader* shader, const Texture* texture)
	{
		VIVIUM_SCOPE;

		quad->GetVertexBuffer()->Bind();
		const IndexBuffer* ib = Quad::GetIndexBuffer(); ib->Bind();

		// Bind texture
		uint8_t slot = GetTextureSlot();
		texture->Bind(slot);

		// Bind shader
		shader->Bind();
		shader->SetUniform1i("u_Texture", slot);
		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);

		// Unbind texture
		Texture::Unbind();
		FreeTextureSlot(slot);
	}

	void Renderer::Submit(const Quad* quad, Shader* shader, const Texture* texture, uint8_t slot)
	{
		VIVIUM_SCOPE;

		quad->GetVertexBuffer()->Bind();
		const IndexBuffer* ib = Quad::GetIndexBuffer(); ib->Bind();

		// Bind texture
		texture->Bind(slot);

		// Bind shader
		shader->Bind();
		shader->SetUniform1i("u_Texture", slot);
		shader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		shader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		shader->SetUniform1f("u_Time", Timer::GetTime());

		glDrawElements(GL_TRIANGLES, ib->GetCount(), ib->GetType(), nullptr);
	}

	void Renderer::Submit(const Text* text)
	{
		VIVIUM_SCOPE;

		text->Render();
	}

	void Renderer::Submit(Button* btn)
	{
		VIVIUM_SCOPE;

		// Get texture currently being used (if there is a texture)
		const Texture* current_texture = btn->CurrentTexture().get();
		Shader* current_shader = btn->CurrentShader().get();

		if (current_texture != nullptr) {
			// Render with texture
			Renderer::Submit(btn->quad.get(), current_shader, current_texture);
		}
		else {
			// Render without texture
			Renderer::Submit(btn->quad.get(), current_shader);
		}

		// Render text of button
		Renderer::Submit(btn->text.get());
	}

	void Renderer::Submit(Animation* animation)
	{
		VIVIUM_SCOPE;

		Renderer::Submit(animation->quad.get(), animation->shader.get(), animation->GetAtlas().get());
	}

	void Renderer::Submit(TextInput* text_input)
	{
		VIVIUM_SCOPE;

		if (text_input->bg_texture != nullptr) {
			Renderer::Submit(text_input->quad.get(), text_input->bg_shader.get(), text_input->bg_texture.get());
		}
		else {
			Renderer::Submit(text_input->quad.get(), text_input->bg_shader.get());
		}

		Renderer::Submit(text_input->GetText().get());

		// Display typing bar
		if (text_input->GetIsTyping()) {
			Renderer::Submit(text_input->GetTypingBar().get());
		}
	}

	void Renderer::Submit(Slider* slider)
	{
		VIVIUM_SCOPE;

		slider->bar_shader->Bind();
		slider->bar_shader->SetUniform1f("u_Value", slider->GetValue());

		if (slider->bar_texture != nullptr) {
			Renderer::Submit(slider->m_BarQuad.get(), slider->bar_shader.get(), slider->bar_texture.get());
		}
		else {
			Renderer::Submit(slider->m_BarQuad.get(), slider->bar_shader.get());
		}

		if (slider->slider_texture != nullptr) {
			Renderer::Submit(slider->m_SliderQuad.get(), slider->slider_shader.get(), slider->slider_texture.get());
		}
		else {
			Renderer::Submit(slider->m_SliderQuad.get(), slider->slider_shader.get());
		}
	}

	void Renderer::Submit(Sprite* sprite)
	{
		VIVIUM_SCOPE;

		Renderer::Submit(sprite->quad.get(), sprite->shader.get(), sprite->texture.get());
	}

	void Renderer::DrawCircle(const Vector2<float>& center, float radius, const RGBColor& color)
	{
		VIVIUM_SCOPE;

		m_CircleShader->Bind();
		m_CircleShader->SetUniform3f("u_Color", color);
		m_CircleShader->SetUniformMat4fv("u_ProjMat", camera->GetProjMat());
		m_CircleShader->SetUniformMat4fv("u_ViewMat", camera->GetViewMat());
		m_CircleShader->SetUniform1f("u_Time", Timer::GetTime());

		float vertices[16] = {
			center.x - radius, center.y - radius, 0.0f, 0.0f, // Bottom left
			center.x + radius, center.y - radius, 1.0f, 0.0f, // Bottom right
			center.x + radius, center.y + radius, 1.0f, 1.0f, // Top right
			center.x - radius, center.y + radius, 0.0f, 1.0f  // Top left
		};

		uint8_t indices[6] = { 0, 1, 2, 2, 3, 0 };

		static const BufferLayout layout = {
			GLSLDataType::VEC2, // Position
			GLSLDataType::VEC2  // Tex coords
		};

		VertexBuffer vb(vertices, 16, layout);
		IndexBuffer ib(indices, 6);

		vb.Bind();
		ib.Bind();

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
	}
}