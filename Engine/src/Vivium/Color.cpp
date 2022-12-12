#include "Color.h"

namespace Vivium {
	RGBColor RGBColor::RED =		RGBColor(255,   0,   0);
	RGBColor RGBColor::GREEN =		RGBColor(  0, 255,   0);
	RGBColor RGBColor::BLUE =		RGBColor(  0,   0, 255);
	RGBColor RGBColor::YELLOW =		RGBColor(255, 255,   0);
	RGBColor RGBColor::TEAL =		RGBColor(  0, 255, 255);
	RGBColor RGBColor::PURPLE =		RGBColor(255,   0, 255);
	RGBColor RGBColor::ORANGE =		RGBColor(255, 162,   0);
	RGBColor RGBColor::GRAY =		RGBColor(105, 105, 105);
	RGBColor RGBColor::DARKGRAY =	RGBColor( 40,  40,  40);
	RGBColor RGBColor::BLACK =		RGBColor(  0,   0,   0);
	RGBColor RGBColor::WHITE =		RGBColor(255, 255, 255);
	RGBColor RGBColor::BROWN =		RGBColor(128,  64,   0);

	char RGBColor::m_DigitToHex(const uint8_t& digit) const {
		// kinda ugly but works well enough
		switch ((int)digit) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			return static_cast<char>(digit + '0');
		case 10:
		case 11:
		case 12:
		case 13: 
		case 14:
		case 15:
			return static_cast<char>(digit - 10 + 'A');
		default:
			LogError("Invalid digit value of {}", (int)digit); return '?';
		}
	}
	RGBColor::RGBColor(float r, float g, float b)
		: r(r), g(g), b(b)
	{}

	RGBColor::RGBColor(uint8_t r, uint8_t g, uint8_t b)
		: r(float(r) / 255.0f), g(float(g) / 255.0f), b(float(b) / 255.0f)
	{}

	RGBColor::RGBColor(int r, int g, int b)
		: r(float(r) / 255.0f), g(float(g) / 255.0f), b(float(b) / 255.0f)
	{}

	RGBColor::RGBColor(const std::string& hex_string)
	{
		// TODO: this is just kinda dumb, like if im gonna use stoi
		//	might as well split the string up into 3 parts and stoi each part
		std::regex cleaner("^#|0x"); // Match # or 0x at start of string
		std::string cleaned = std::regex_replace(hex_string, cleaner, "");

		uint32_t hex_value = stoi(cleaned, 0, 16);
		uint8_t ri = hex_value >> 16;
		uint8_t gi = (hex_value >> 8) - (ri << 8);
		uint8_t bi = hex_value - (gi << 8) - (ri << 16);

		r = ri / 255.0f;
		g = gi / 255.0f;
		b = bi / 255.0f;
	}
	 
	std::string RGBColor::GetHex() const
	{
		std::array<float, 3> values = { r, g, b };

		std::string hex_string;

		for (float v : values) {
			uint8_t byte_value = v * 255;
			uint8_t second_digit = v / 16;
			uint8_t first_digit = byte_value - second_digit * 16;

			hex_string += m_DigitToHex(second_digit);
			hex_string += m_DigitToHex(first_digit);
		}

		return hex_string;
	}

	void RGBColor::Write(Serialiser& s) const
	{
		s.Write(r);
		s.Write(g);
		s.Write(b);
	}

	void RGBColor::Read(Serialiser& s)
	{
		s.Read(&r);
		s.Read(&g);
		s.Read(&b);
	}
}

namespace std {
	template <>
	struct formatter<Vivium::RGBColor> : formatter<string> {
		auto format(Vivium::RGBColor color, format_context& ctx) {
			return formatter<string>::format(
				std::format("#{}", color.GetHex()), ctx);
		}
	};
}
