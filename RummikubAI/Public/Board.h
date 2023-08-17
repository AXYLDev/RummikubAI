#pragma once
#include "Utility.h"
#include <chrono>

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

	bool TilePossible(const std::vector<Tile>& board, Tile t) {
		if (t.color == Color::Joker) return true;
		enum class Bit : uint8_t {
			Minus2 = 1, Minus1 = 2, Equal = 4, Plus1 = 8, Plus2 = 16
		};
		Bit color = Bit::Equal;
		uint8_t numberColorMask = (uint8_t)t.color;
		uint8_t jokerCount = 0;
		for (size_t j = 0; j < board.size(); j++) {
			Tile ot = board[j];
			if (ot.color == Color::Joker) {
				jokerCount++;
				if (jokerCount >= 2) return true;
			}
			else {
				if (ot.color == t.color) {
					int8_t delta = (int8_t)t.number - (int8_t)ot.number;
					if (delta >= -2 && delta <= 2) *(uint8_t*)&color |= 1ui8 << (delta + 2);
				}
				else if (ot.number == t.number) {
					numberColorMask |= (uint8_t)ot.color;
				}
			}
		}
		uint8_t numberCount = 0;
		for (size_t i = 0; i < 5; i++) {
			if ((uint8_t)color & (1ui8 << i)) {
				numberCount++;
				if (numberCount + jokerCount >= 3) break;
			}
			else numberCount = 0;
		}
		uint8_t colorCount = 0;
		for (size_t i = 0; i < 4; i++)
			if (numberColorMask & (1 << i))
				colorCount++;
		// Check possible
		if (numberCount + jokerCount < 3
			&& colorCount + jokerCount < 3)
			return false;
		return true;
	}
	bool FastPossibleCheck(const std::vector<Tile>& board) {
		for (size_t i = 0; i < board.size(); i++) {
			if (!TilePossible(board, board[i])) return false;
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
		/* Slow but thorough way
		for (size_t numTilesToPlay = m_hand.size(); numTilesToPlay > 0; numTilesToPlay--) {
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
		*/
		// Fast way
		auto start = std::chrono::system_clock::now();
		auto lastIncrease = std::chrono::system_clock::now();
		uint32_t c = 0;
		Move move;
		std::vector<Tile> effectiveHand;
		{
			std::vector<Tile> effectiveBoard = m_tiles;
			effectiveBoard.insert(effectiveBoard.end(), m_hand.begin(), m_hand.end());
			for (size_t i = 0; i < m_hand.size(); i++)
				if (TilePossible(effectiveBoard, m_hand[i])) effectiveHand.emplace_back(m_hand[i]);
		}
		float increaseInterval = effectiveHand.size() >= 5 ? 2 : 10.f / effectiveHand.size();
		for (size_t numTilesToPlay = 1; numTilesToPlay <= effectiveHand.size()
			&& std::chrono::duration<double>(std::chrono::system_clock::now() - start).count() < 10; c++) {
			std::vector<Tile> hand;
			hand.reserve(numTilesToPlay);
			{
				std::vector<Tile> tempHand = m_hand;
				for (size_t i = 0; i < numTilesToPlay; i++) {
					int j = Rand(tempHand.size());
					hand.emplace_back(tempHand[j]);
					tempHand.erase(tempHand.begin() + j);
				}
			}
			std::vector<Tile> board = m_tiles;
			board.insert(board.end(), hand.begin(), hand.end());
			// Attempt to find move
			Move m = TryFindMove(board, numTilesToPlay);
			if (!m.Null()) {
				move = m;
				numTilesToPlay++;
			}
			else if (std::chrono::duration<double>(std::chrono::system_clock::now() - lastIncrease).count() > increaseInterval) {
				numTilesToPlay++;
				lastIncrease = std::chrono::system_clock::now();
			}
		}
		std::cout << "Finished processing after " << c << (c == 1 ? " iteration.\n" : " iterations.\n");
		if (!move.Null()) {
			std::cout << "Solution found:\n";
			// Remove tiles from hand
			for (size_t i = 0; i < move.playedTiles.size(); i++) {
				m_hand.erase(std::find(m_hand.begin(), m_hand.end(), move.playedTiles[i]));
			}
			// Add tiles to board
			size_t n = m_tiles.size() + move.playedTiles.size();
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
	Move TryFindMove(std::vector<Tile> board, int numPlayed) {
		// Fast possible check
		for (size_t i = board.size() - numPlayed; i < board.size(); i++) {
			if (!TilePossible(board, board[i])) return Move();
		}
		Move move;
		move.playedTiles.insert(move.playedTiles.end(), board.begin() + (board.size() - numPlayed), board.end());
		// Shuffle
		{
			std::vector<Tile> shuffled;
			shuffled.reserve(board.size());
			while (board.size() > 0) {
				int i = Rand(board.size());
				shuffled.emplace_back(board[i]);
				board.erase(board.begin() + i);
			}
			board = std::move(shuffled);
		}
		//if (!FastPossibleCheck(board)) return Move();
		// Find sln
		move.sln.emplace_back(Run());
		while (board.size() > 0) {
			Run& run = *move.sln.rbegin();
			size_t i;
			for (i = 0; i < board.size(); i++) {
				if (run.AddIfCan(board[i])) {
					board.erase(board.begin() + i);
					i = 0;
					break;
				}
			}
			if (i != 0) {
				if (run.tiles.size() >= 3) {
					int j = Rand(board.size());
					move.sln.emplace_back(Run(board[j]));
					board.erase(board.begin() + j);
				}
				else return Move();
			}
		}
		if (move.sln.rbegin()->tiles.size() >= 3) return move;
		return Move();
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