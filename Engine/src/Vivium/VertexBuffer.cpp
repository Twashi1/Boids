#include "VertexBuffer.h"

namespace Vivium {
	const GLSLDataType GLSLDataType::FLOAT =	{ sizeof(float),			GL_FLOAT,		 1  };
	const GLSLDataType GLSLDataType::VEC2 =		{ sizeof(float) * 2,		GL_FLOAT,		 2  };
	const GLSLDataType GLSLDataType::VEC3 =		{ sizeof(float) * 3,		GL_FLOAT,		 3  };
	const GLSLDataType GLSLDataType::VEC4 =		{ sizeof(float) * 4,		GL_FLOAT,		 4  };
	const GLSLDataType GLSLDataType::INT =		{ sizeof(int),				GL_INT,			 1  };
	const GLSLDataType GLSLDataType::IVEC2 =	{ sizeof(int) * 2,			GL_INT,			 2  };
	const GLSLDataType GLSLDataType::IVEC3 =	{ sizeof(int) * 3,			GL_INT,			 3  };
	const GLSLDataType GLSLDataType::IVEC4 =	{ sizeof(int) * 4,			GL_INT,			 4  };
	const GLSLDataType GLSLDataType::UINT =		{ sizeof(unsigned int),		GL_UNSIGNED_INT, 1  };
	const GLSLDataType GLSLDataType::UVEC2 =	{ sizeof(unsigned int) * 2, GL_UNSIGNED_INT, 2  };
	const GLSLDataType GLSLDataType::UVEC3 =	{ sizeof(unsigned int) * 3, GL_UNSIGNED_INT, 3  };
	const GLSLDataType GLSLDataType::UVEC4 =	{ sizeof(unsigned int) * 4, GL_UNSIGNED_INT, 4  };
	const GLSLDataType GLSLDataType::BOOL =		{ sizeof(bool),				GL_BOOL,		 1  };
	const GLSLDataType GLSLDataType::MAT4x4 =	{ sizeof(float) * 4 * 4,	GL_FLOAT,		 16 };

	GLSLDataType::GLSLDataType(unsigned int size, unsigned int type, unsigned int component_count)
		: size(size), gl_type(type), component_count(component_count)
	{}

	GLSLDataType::GLSLDataType(const GLSLDataType & other)
		: size(other.size), gl_type(other.gl_type), component_count(other.component_count)
	{}

	GLSLDataType::GLSLDataType(GLSLDataType&& other) noexcept
		: size(std::move(other.size)), gl_type(std::move(other.gl_type)), component_count(std::move(other.component_count))
	{}

	VertexBuffer::VertexBuffer(const std::vector<float>& data, const BufferLayout& layout)
		: m_ID(0), m_Layout(layout)
	{
		VIVIUM_SCOPE;
		
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		const std::vector<BufferElement>& elements = m_Layout.GetElements();

		for (int i = 0; i < elements.size(); i++) {
			const BufferElement& element = elements[i];

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,								// Layout location
				element.type.component_count,	// Amount of components of type
				element.type.gl_type,			// GL base type
				GL_FALSE,						// Is normalised
				m_Layout.GetStride(),			// Stride
				(const void*)element.offset		// Offset
			);
		}
	}

	VertexBuffer::VertexBuffer(const float* data, const std::size_t& count, const BufferLayout& layout)
		: m_ID(0), m_Layout(layout)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_DYNAMIC_DRAW);

		const std::vector<BufferElement>& elements = m_Layout.GetElements();

		for (int i = 0; i < elements.size(); i++) {
			const BufferElement& element = elements[i];

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,								// Layout location
				element.type.component_count,	// Amount of components of type
				element.type.gl_type,			// GL base type
				GL_FALSE,						// Is normalised
				m_Layout.GetStride(),			// Stride
				(const void*)element.offset		// Offset
			);
		}
	}

	VertexBuffer::VertexBuffer(const void* data, const std::size_t& size, const BufferLayout& layout)
		: m_ID(0), m_Layout(layout)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

		const std::vector<BufferElement>& elements = m_Layout.GetElements();

		for (int i = 0; i < elements.size(); i++) {
			const BufferElement& element = elements[i];

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,								// Layout location
				element.type.component_count,	// Amount of components of type
				element.type.gl_type,			// GL base type
				GL_FALSE,						// Is normalised
				m_Layout.GetStride(),				// Stride
				(const void*)element.offset		// Offset
			);
		}
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void VertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		const std::vector<BufferElement>& elements = m_Layout.GetElements();

		for (int i = 0; i < elements.size(); i++) {
			const BufferElement& element = elements[i];

			// glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,								// Layout location
				element.type.component_count,	// Amount of components of type
				element.type.gl_type,			// GL base type
				GL_FALSE,						// Is normalised
				m_Layout.GetStride(),			// Stride
				(const void*)element.offset		// Offset
			);
		}
	}

	const BufferLayout& VertexBuffer::GetLayout() const { return m_Layout; }

	const GLuint& VertexBuffer::GetID() const { return m_ID; }

	void VertexBuffer::Set(const std::vector<float>& data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), &data[0]);
	}

	void VertexBuffer::Set(const void* data, const std::size_t& size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void VertexBuffer::Set(unsigned int element_index, const void* data, const std::size_t& size)
	{
		// Changing the data of only a specific element is pretty difficult, so lots of code here to get the correct indices and etc.
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		const std::vector<BufferElement>& elements = m_Layout.GetElements();
		
		// Offset in bytes from the start of a vertex to the data for this element
		int offset = elements[element_index].offset;
		// Size in bytes of the element
		int element_size = elements[element_index].type.size;
		// Bytes per vertex
		int stride = m_Layout.GetStride();
		int total_vertices = size / element_size;

		for (int i = 0; i < total_vertices; i++) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, element_size, (char*)data + (i * element_size));

			// Offset is from start of the data to the position of the element in the first vertex, so we add stride to move to the next vertex
			offset += stride;
		}
	}

	void* VertexBuffer::StartMap()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);

		return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}

	void VertexBuffer::EndMap()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	BufferElement::BufferElement(const GLSLDataType& type)
		: type(type), offset(0) {}

	BufferElement::BufferElement(const BufferElement& other)
		: type(other.type), offset(other.offset)
	{}

	// Calculates offsets and stride
	void BufferLayout::m_Calculate() {
		unsigned int running_offset = 0;

		for (BufferElement& element : m_Elements) {
			element.offset = running_offset;
			running_offset += element.type.size;
			m_Stride += element.type.size;
		}
	}

	const std::vector<BufferElement>& BufferLayout::GetElements() const { return m_Elements; }

	unsigned int BufferLayout::GetStride() const { return m_Stride; }

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements)
	{
		m_Calculate();
	}
}
