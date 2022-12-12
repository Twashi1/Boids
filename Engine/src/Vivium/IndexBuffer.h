#pragma once

#include "Core.h"
#include "Logger.h"

namespace Vivium {
	class VIVIUM_API IndexBuffer {
		GLuint m_ID;
		GLushort m_Count;	// Amount of indices
		GLenum m_GLType;	// GLSL data type

	public:
		IndexBuffer(const std::vector<uint16_t>& indices);
		IndexBuffer(const std::vector<uint8_t>& indices);
		IndexBuffer(const uint8_t* data, const std::size_t& count);
		IndexBuffer(const uint16_t* data, const std::size_t& count);
		IndexBuffer(const void* data, const std::size_t& size, const std::size_t& count, GLenum gl_type);
		~IndexBuffer();

		const GLuint& GetID() const;
		const GLushort& GetCount() const;
		const GLenum& GetType() const;

		static void Unbind();
		void Bind() const;
	};
}