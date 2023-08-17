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
		case 'j':
		case 'J':
			tiles.emplace_back(Tile{ 0, Color::Joker });
			continue;
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

size_t Rand(size_t num) {
	size_t r = size_t((float)rand() / RAND_MAX * num);
	return r == num ? r - 1 : r;
}