#include "Framebuffer.h"

#include "Renderer.h"

namespace Vivium {
	Framebuffer::Framebuffer(const Vector2<int>& dim)
		: m_Width(dim.x), m_Height(dim.y)
	{
		Resize(dim.x, dim.y);
	}

	Framebuffer::Framebuffer(int width, int height)
		: m_Width(width), m_Height(height)
	{
		Resize(width, height);
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
	}

	void Framebuffer::Clear() const
	{
		static const float clearcolor[] = {0.0f, 0.0f, 0.0f, 0.0f};

		// Clear the color attachment
		glClearTexImage(m_ColorAttachment, 0, GL_RGBA, GL_FLOAT, clearcolor);
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}

	void Framebuffer::SetSlot(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
	}

	int Framebuffer::GetWidth() const
	{
		return m_Width;
	}

	int Framebuffer::GetHeight() const
	{
		return m_Height;
	}

	Vector2<int> Framebuffer::GetDim() const
	{
		return Vector2<int>(m_Width, m_Height);
	}

	GLuint Framebuffer::GetColorAttachment() const
	{
		return m_ColorAttachment;
	}

	void Framebuffer::Resize(int width, int height)
	{
		VIVIUM_SCOPE;

		this->m_Width = width;
		this->m_Height = height;

		// If our object is uninitialised
		if (m_ID == 0) {
			glCreateFramebuffers(1, &m_ID);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		// If colour attachment uninitialised
		if (m_ColorAttachment == 0) {
			glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		}

		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { LogError("Framebuffer is incomplete"); }

		// Unbinds
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
}
