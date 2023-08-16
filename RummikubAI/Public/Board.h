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
					m_tiles.insert(m_tiles.end(), sln[i].begin(), sln[i].end());
					// Print solution
					std::string out;
					for (size_t j = 0; j < sln[i].size(); j++) {
						out += (std::string)sln[i][j] + " ";
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
	std::vector<std::vector<Tile>> ChooseN(std::vector<Tile> board, std::vector<Tile> hand, int n) {
		if (n == 0) return IsValid(board);
		for (size_t i = 0; i < hand.size() - n; i++) {
			board.emplace_back(hand[i]);
			auto sln = ChooseN(board, std::vector<Tile>(hand.begin() + i + 1, hand.end()), n - 1);
			if (!sln.empty()) return sln;
			board.pop_back();
		}
		return std::vector<std::vector<Tile>>();
	}
	std::vector<std::vector<Tile>> IsValid(std::vector<Tile> board) {
		std::vector<std::vector<Tile>> sln;
		sln.emplace_back(std::vector<Tile>());
		return ShuffleBoard(sln, std::vector<Tile>(), board);
	}
	std::vector<std::vector<Tile>> ShuffleBoard(std::vector<std::vector<Tile>> sln, std::vector<Tile> board, std::vector<Tile> tiles) {
		if (tiles.size() == 0) return sln.rbegin()->size() >= 3 ? sln : std::vector<std::vector<Tile>>();
		for (size_t i = 0; i < tiles.size(); i++) {
			board.emplace_back(tiles[i]);
			Tile t = tiles[i];
			if (AddToSolution(sln, t)) {
				tiles.erase(tiles.begin() + i);
				auto osln = ShuffleBoard(sln, board, tiles);
				if (!osln.empty()) return osln;
				tiles.insert(tiles.begin() + i, t);
				sln.rbegin()->pop_back();
				if (sln.rbegin()->size() == 0 && sln.size() > 1) sln.pop_back();
			}
			board.pop_back();
		}
		return std::vector<std::vector<Tile>>();
	}
	bool AddToSolution(std::vector<std::vector<Tile>>& sln, Tile t) {
		std::vector<Tile>& run = sln[sln.size() - 1];
		if (run.size() == 0 || t.color == Color::Joker
			|| (run.size() == 1 && run[0].color == Color::Joker)) {
			run.emplace_back(t);
			return true;
		}
		if (run.size() == 1) {
			// Create run by adding to last tile
			if ((run[0].color == t.color && t.number == run[0].number + 1)
				|| (run[0].number == t.number && run[0].color != t.color)) {
				run.emplace_back(t);
				return true;
			}
		} else {
			// Add to existing run
			// Joker?
			if (run[0].color == Color::Joker) {
				if (run.size() == 2) {
					if ((run[1].color == t.color && t.number == run[1].number + 1)
						|| (run[1].number == t.number && run[1].color != t.color)) {
						run.emplace_back(t);
						return true;
					}
				}
				else {
					// Is same color run?
					if (run[1].color == run[2].color
						&& t.color == run[1].color && t.number == run[1].number + run.size() - 1) {
						run.emplace_back(t);
						return true;
					}
					// Is same number run?
					else if (run[1].number == run[2].number && t.number == run[1].number) {
						if (run.size() < 4) {
							size_t i;
							for (i = 0; i < run.size(); i++)
								if (run[i].color == t.color) break;
							if (i == run.size()) {
								run.emplace_back(t);
								return true;
							}
						}
					}
				}
			}
			else if (run[1].color == Color::Joker) {
				if (run.size() == 2) {
					if ((run[0].color == t.color && t.number == run[0].number + 2)
						|| (run[0].number == t.number && run[0].color != t.color)) {
						run.emplace_back(t);
						return true;
					}
				}
				else {
					// Is same color run?
					if (run[0].color == run[2].color
						&& t.color == run[0].color && t.number == run[0].number + run.size()) {
						run.emplace_back(t);
						return true;
					}
					// Is same number run?
					else if (run[0].number == run[2].number && t.number == run[0].number) {
						if (run.size() < 4) {
							size_t i;
							for (i = 0; i < run.size(); i++)
								if (run[i].color == t.color) break;
							if (i == run.size()) {
								run.emplace_back(t);
								return true;
							}
						}
					}
				}
			}

			// Not joker?
			else {
				// Is same color run?
				if (run[0].color == run[1].color
					&& t.color == run[0].color && t.number == run[0].number + run.size()) {
					run.emplace_back(t);
					return true;
				}
				// Is same number run?
				else if (run[0].number == run[1].number && t.number == run[0].number) {
					if (run.size() < 4) {
						size_t i;
						for (i = 0; i < run.size(); i++)
							if (run[i].color == t.color) break;
						if (i == run.size()) {
							run.emplace_back(t);
							return true;
						}
					}
				}
			}
		}
		if (run.size() >= 3) {
			std::vector<Tile> newRun;
			newRun.emplace_back(t);
			sln.emplace_back(newRun);
			return true;
		}
		else {
			if (run.size() == 2) {
				auto& lrun = sln[sln.size() - 2];
				if (lrun.size() >= 4 && lrun[lrun.size() - 1].color == Color::Joker) {
					run.emplace_back(lrun[lrun.size() - 1]);
					lrun.erase(lrun.begin() + (lrun.size() - 1));
				}
				std::vector<Tile> newRun;
				newRun.emplace_back(t);
				sln.emplace_back(newRun);
				return true;
			}
			return false;
		}
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