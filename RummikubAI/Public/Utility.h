#pragma once
#include <vector>
#include <sstream>
#include <iostream>
#include "Tile.h"

// Ask the user to enter a list of tiles
std::vector<Tile> InputTileList() {
	char input[256];
	std::cin.getline(input, 256);
	std::stringstream ss(input);
	std::vector<Tile> tiles;
	while (!ss.eof()) {
		char color;
		ss >> color;
		Tile tile;
		switch (color) {
		case 'B':
			tile.color = Color::Black;
			break;
		case 'b':
			tile.color = Color::Blue;
			break;
		case 'r':
		case 'R':
			tile.color = Color::Red;
			break;
		case 'y':
		case 'Y':
			tile.color = Color::Yellow;
			break;
		default:
			continue;
		}
		int number;
		ss >> number;
		if (number < 1 || number > 13) continue;
		tile.number = number;
		tiles.emplace_back(tile);
	}

	return tiles;
}