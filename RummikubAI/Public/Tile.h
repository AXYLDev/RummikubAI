#pragma once
#include <cstdint>

enum class Color : char {
	Black, Blue, Red, Yellow
};

struct Tile {
	operator std::string() {
		std::stringstream out;
		switch (color) {
		case Color::Black:
			out << "B"; break;
		case Color::Blue:
			out << "b"; break;
		case Color::Red:
			out << "r"; break;
		case Color::Yellow:
			out << "y"; break;
		}
		out << number;
		return out.str();
	}
	uint8_t number;
	Color color;
};
