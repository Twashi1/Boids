#pragma once

#include "Core.h"
#include "Logger.h"
#include "Serialiser.h"

namespace Vivium {
	struct VIVIUM_API RGBColor : public Streamable {
	private:
		char m_DigitToHex(const uint8_t& digit) const;

	public:
		float r, g, b;

		RGBColor(float r, float g, float b);
		RGBColor(uint8_t r, uint8_t g, uint8_t b);
		RGBColor(int r, int g, int b);
		RGBColor(const std::string& hex_string);

		std::string GetHex() const;

		void Write(Serialiser& s) const override;
		void Read(Serialiser& s) override;

		friend bool operator==(const RGBColor& a, const RGBColor& b) {
			return a.r == b.r && a.g == b.g && a.b == b.b;
		}

		friend bool operator!=(const RGBColor& a, const RGBColor& b) {
			return a.r != b.r || a.g != b.g || a.b != b.b;
		}

		static RGBColor RED;
		static RGBColor GREEN;
		static RGBColor BLUE;
		static RGBColor YELLOW;
		static RGBColor TEAL;
		static RGBColor PURPLE;
		static RGBColor ORANGE;
		static RGBColor GRAY;
		static RGBColor DARKGRAY;
		static RGBColor BLACK;
		static RGBColor WHITE;
		static RGBColor BROWN;
	};
}