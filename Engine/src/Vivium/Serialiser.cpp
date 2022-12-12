#include "Serialiser.h"

namespace Vivium {
	Serialiser::Serialiser(int flags)
		: m_Flags(flags)
	{
		m_Stream = Stream();
	}

	void Serialiser::BeginRead(const char* path) {
		if (!std::filesystem::exists(path)) { LogError("Couldn't find file at path: {}", path); }

		int stripped_flags = (m_Flags & ~Stream::Flags::TRUNC) & ~Stream::Flags::APPEND;

		m_Stream.in = new std::ifstream(path, stripped_flags);

		if (!m_Stream.in->is_open()) { LogError("File was not opened successfully"); }
	}

	void Serialiser::BeginWrite(const char* path) {
		m_Stream.out = new std::ofstream(path, m_Flags);

		if (!m_Stream.out->is_open()) { LogError("File was not opened/not created successfully"); }
	}

	void Serialiser::EndRead() {
		m_Stream.in->close();
		delete m_Stream.in;
		m_Stream.in = nullptr;
	}

	void Serialiser::EndWrite() {
		m_Stream.out->close();
		delete m_Stream.out;
		m_Stream.out = nullptr;
	}
}