#pragma once
#include <cstdint>

enum class Color : uint8_t {
	Black = 1, Blue = 2, Red = 4, Yellow = 8, Joker
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
		case Color::Joker:
			return "j";
		}
		out << (int)number;
		return out.str();
	}
	uint8_t number;
	Color color;

	bool operator==(const Tile& other) const {
		return *(uint16_t*)this == *(uint16_t*)&other;
	}
	bool operator!=(const Tile& other) const {
		return *(uint16_t*)this != *(uint16_t*)&other;
	}
};


struct Run {
	Run() {}
	Run(Tile t) : tiles{ t } {}

	enum class Type : char {
		None, SameColor, SameNumber
	};
	Type type = Type::None;
	union TypeInfo {
		struct ColorInfo {
			Color color;
			uint8_t number0;
		} color;
		struct NumberInfo {
			uint8_t number, colorMask;
		} number;
	} typeInfo;
	std::vector<Tile> tiles;

	bool AddIfCan(Tile t) {
		if (t.color != Color::Joker && tiles.size() > 0) {
			switch (type) {
			case Type::None: {
				if (tiles.size() == 1 && tiles[0].color != Color::Joker) {
					const auto t0 = tiles[0];
					if (t0 == t) return false;
					if (t0.color == t.color && t.number == t0.number + 1) {
						type = Type::SameColor;
						typeInfo.color.color = t.color;
						typeInfo.color.number0 = t0.number;
					}
					else if (t0.number == t.number && t0.color != t.color) {
						type = Type::SameNumber;
						typeInfo.number.number = t.number;
						typeInfo.number.colorMask = (uint8_t)t0.color | (uint8_t)t.color;
					}
					else return false;
				}
				// If Joker present
				else {
					TypeInfo::ColorInfo color;
					TypeInfo::NumberInfo number { 0, 0 };
					for (size_t i = 0; i < tiles.size(); i++) {
						Tile t = tiles[i];
						if (t.color != Color::Joker) {
							color.color = t.color;
							color.number0 = t.number - i;
							number.number = t.number;
							number.colorMask = (uint8_t)t.color;
							break;
						}
					}
					// If not all jokers
					if (number.number != 0) {
						// Same color
						if (t.color == color.color
							&& t.number == color.number0 + tiles.size()) {
							typeInfo.color = color;
						}
						// Same number
						else if (t.number == number.number
							&& !(typeInfo.number.colorMask & (uint8_t)t.color) && tiles.size() != 4) {
							number.colorMask |= (uint8_t)t.color;
							typeInfo.number = number;
						}
						// Cannot add
						else return false;
					}
				}
				break;
			}
			case Type::SameColor:
				if (t.color != typeInfo.color.color
					|| t.number != typeInfo.color.number0 + tiles.size()) return false;
				break;
			case Type::SameNumber:
				if (t.number != typeInfo.number.number
					|| (typeInfo.number.colorMask & (uint8_t)t.color) || tiles.size() == 4) return false;
				typeInfo.number.colorMask |= (uint8_t)t.color;
				break;
			}
		}
		tiles.emplace_back(t);
		return true;
	}
};