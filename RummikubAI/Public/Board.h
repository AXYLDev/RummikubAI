#pragma once
#include "Utility.h"

class Board {
public:
	Board() {
		// Ask for starting tiles
		while (true) {
			std::cout << "Enter 14 tiles for the initial hand of the AI.\n";
			m_hand = InputTileList();
			if (m_hand.size() == 14) break;
			std::cout << "You did not enter 14 valid tiles.\n";
		}
	}
	void AddTiles(std::vector<Tile> newTiles) {
		m_tiles.insert(m_tiles.end(), newTiles.begin(), newTiles.end());
	}
	void MakeMove() {
		for (size_t numTilesToPlay = m_hand.size(); numTilesToPlay > 1; numTilesToPlay--) {
			auto sln = ChooseN(m_tiles, m_hand, numTilesToPlay);
			if (!sln.empty()) {
				std::cout << "Solution found:\n";
				// Add tiles to board
				size_t n = m_tiles.size() + numTilesToPlay;
				m_tiles.clear();
				m_tiles.reserve(n);
				for (size_t i = 0; i < sln.size(); i++) {
					m_tiles.insert(m_tiles.end(), sln[i].tiles.begin(), sln[i].tiles.end());
					// Print solution
					std::string out;
					for (size_t j = 0; j < sln[i].tiles.size(); j++) {
						out += (std::string)sln[i].tiles[j] + " ";
					}
					std::cout << out << "\n";
				}
				return;
			}
		}
		// No solution, ask to pick up tile
		std::vector<Tile> pickedUp;
		while (true) {
			std::cout << "Cannot make a move, please pick up a tile and enter it.\n";
			pickedUp = InputTileList();
			if (!pickedUp.empty()) break;
			std::cout << "Invalid tile.\n";
		}
		m_hand.emplace_back(pickedUp[0]);
	}
	std::vector<Run> ChooseN(std::vector<Tile> board, std::vector<Tile> hand, int n) {
		if (n == 0) return IsValid(board);
		for (size_t i = 0; i < hand.size() - n; i++) {
			board.emplace_back(hand[i]);
			auto sln = ChooseN(board, std::vector<Tile>(hand.begin() + i + 1, hand.end()), n - 1);
			if (!sln.empty()) return sln;
			board.pop_back();
		}
		return std::vector<Run>();
	}
	std::vector<Run> IsValid(std::vector<Tile> board) {
		std::vector<Run> sln;
		sln.emplace_back(Run());
		return ShuffleBoard(sln, std::vector<Tile>(), board);
	}
	std::vector<Run> ShuffleBoard(std::vector<Run> sln, std::vector<Tile> board, std::vector<Tile> tiles) {
		if (tiles.size() == 0) return sln.rbegin()->tiles.size() >= 3 ? sln : std::vector<Run>();
		for (size_t i = 0; i < tiles.size(); i++) {
			board.emplace_back(tiles[i]);
			Tile t = tiles[i];
			// Store for if remove needed later
			Run run = *sln.rbegin();
			if (AddToSolution(sln, t)) {
				tiles.erase(tiles.begin() + i);
				auto osln = ShuffleBoard(sln, board, tiles);
				if (!osln.empty()) return osln;
				tiles.insert(tiles.begin() + i, t);
				// Remove from solution
				if (sln.rbegin()->tiles.size() == 1 && sln.size() > 1) sln.pop_back();
				else *sln.rbegin() = run;
			}
			board.pop_back();
		}
		return std::vector<Run>();
	}
	bool AddToSolution(std::vector<Run>& sln, Tile t) {
		Run& run = sln[sln.size() - 1];
		if (!run.AddIfCan(t)) {
			if (run.tiles.size() >= 3) {
				sln.emplace_back(Run());
				return true;
			}
			return false;
		}
		return true;
	}

	// Loop through turns
	void Play() {
		while (true) {
			std::cout << "Select an action.\n"
				<< "move - get AI to make a move\n"
				<< "add - add tiles to board (from other's moves)\n";
			std::string action;
			std::cin >> action;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (action == "move") {
				MakeMove();
			}
			else if (action == "add") {
				std::cout << "Enter the tiles to add to the board.\n";
				AddTiles(InputTileList());
			}
		}
	}

private:
	std::vector<Tile> m_tiles;
	std::vector<Tile> m_hand;

};