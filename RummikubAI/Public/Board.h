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

	bool FastPossibleCheck(const std::vector<Tile>& board) {
		for (size_t i = 0; i < board.size(); i++) {
			Tile t = board[i];
			if (t.color == Color::Joker) continue;
			enum class Bit : uint8_t {
				Minus2 = 1, Minus1 = 2, Equal = 4, Plus1 = 8, Plus2 = 16
			};
			Bit color = Bit::Equal;
			uint8_t numberColorMask = (uint8_t)t.color;
			uint8_t jokerCount = 0;
			for (size_t j = 0; j < board.size(); j++) {
				if (i != j) {
					Tile ot = board[j];
					if (ot.color == Color::Joker) {
						jokerCount++;
						if (jokerCount >= 2) return true;
					}
					else {
						if (ot.color == t.color) {
							int8_t delta = t.number - ot.number;
							if (delta >= -2 && delta <= 2) *(uint8_t*)&color |= 1 << (delta + 2);
						}
						else if (ot.number == t.number) {
							numberColorMask |= (uint8_t)ot.color;
						}
					}
				}
			}
			uint8_t colorCount = 0;
			for (size_t i = 0; i < 4; i++)
				if (numberColorMask & (1 << 4))
					colorCount++;
			// Check possible
			if (!((uint8_t)color & ((uint8_t)Bit::Minus2 | (uint8_t)Bit::Minus1))
				&& !((uint8_t)color & ((uint8_t)Bit::Minus1 | (uint8_t)Bit::Plus1))
				&& !((uint8_t)color & ((uint8_t)Bit::Plus1 | (uint8_t)Bit::Plus2))
				&& !(jokerCount == 1 && ((uint8_t)color & ~(uint8_t)Bit::Equal) != 0)
				&& colorCount + jokerCount < 3)
				return false;
		}
		return true;
	}
	struct Move {
		Move() {}
		std::vector<Run> sln;
		std::vector<Tile> playedTiles;
		bool Null() { return sln.empty(); }
	};
	void MakeMove() {
		for (size_t numTilesToPlay = m_hand.size(); numTilesToPlay > 1; numTilesToPlay--) {
			auto move = ChooseN(m_tiles, m_hand, numTilesToPlay);
			if (!move.Null()) {
				std::cout << "Solution found:\n";
				// Remove tiles from hand
				for (size_t i = 0; i < move.playedTiles.size(); i++) {
					m_hand.erase(std::find(m_hand.begin(), m_hand.end(), move.playedTiles[i]));
				}
				// Add tiles to board
				size_t n = m_tiles.size() + numTilesToPlay;
				m_tiles.clear();
				m_tiles.reserve(n);
				for (size_t i = 0; i < move.sln.size(); i++) {
					m_tiles.insert(m_tiles.end(), move.sln[i].tiles.begin(), move.sln[i].tiles.end());
					// Print solution
					std::string out;
					for (size_t j = 0; j < move.sln[i].tiles.size(); j++) {
						out += (std::string)move.sln[i].tiles[j] + " ";
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
	Move ChooseN(std::vector<Tile> board, std::vector<Tile> hand, int n, int totalN = 0) {
		if (totalN == 0) totalN = n;
		if (n == 0) return IsValid(board, totalN);
		for (size_t i = 0; i < hand.size() - n; i++) {
			board.emplace_back(hand[i]);
			auto move = ChooseN(board, std::vector<Tile>(hand.begin() + i + 1, hand.end()), n - 1, totalN);
			if (!move.Null()) return move;
			board.pop_back();
		}
		return Move();
	}
	Move IsValid(std::vector<Tile> board, int playedTiles) {
		if (FastPossibleCheck(board)) {
			Move move;
			move.sln.emplace_back(Run());
			move = ShuffleBoard(move, std::vector<Tile>(), board);
			move.playedTiles.insert(move.playedTiles.end(), board.begin() + (board.size() - playedTiles), board.end());
			return move;
		} else return Move();
	}
	Move ShuffleBoard(Move move, std::vector<Tile> board, std::vector<Tile> tiles) {
		if (tiles.size() == 0) return move.sln.rbegin()->tiles.size() >= 3 ? move : Move();
		for (size_t i = 0; i < tiles.size(); i++) {
			board.emplace_back(tiles[i]);
			Tile t = tiles[i];
			// Store for if remove needed later
			Run run = *move.sln.rbegin();
			if (AddToSolution(move, t)) {
				tiles.erase(tiles.begin() + i);
				auto omove = ShuffleBoard(move, board, tiles);
				if (!omove.Null()) return omove;
				tiles.insert(tiles.begin() + i, t);
				// Remove from solution
				if (move.sln.rbegin()->tiles.size() == 1 && move.sln.size() > 1) move.sln.pop_back();
				else *move.sln.rbegin() = run;
			}
			board.pop_back();
		}
		return Move();
	}
	bool AddToSolution(Move& move, Tile t) {
		Run& run = move.sln[move.sln.size() - 1];
		if (!run.AddIfCan(t)) {
			if (run.tiles.size() >= 3) {
				move.sln.emplace_back(Run(t));
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