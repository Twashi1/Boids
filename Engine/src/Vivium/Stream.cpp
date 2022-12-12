#include "Stream.h"

namespace Vivium {
	Stream::Stream() {}

	Stream::~Stream() {
		if (in != nullptr) { in->close(); }
		if (out != nullptr) { out->close(); }

		delete in;
		delete out;
	}
}
