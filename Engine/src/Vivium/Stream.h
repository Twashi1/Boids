#pragma once

#include "Core.h"
#include "Logger.h"
#include "Vector2.h"
#include "Utils.h"

namespace Vivium {
	class VIVIUM_API Stream {
	public:
		enum Flags : int {
			TEXT = NULL,
			BINARY = std::ios::binary,
			TRUNC = std::ios::trunc,
			APPEND = std::ios::app
		};

		std::ofstream* out = nullptr;
		std::ifstream* in = nullptr;

		Stream();
		~Stream();
	};
}