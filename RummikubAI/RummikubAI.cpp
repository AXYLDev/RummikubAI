// RummikubAI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Public/Board.h"

int main()
{
    std::cout << "Welcome to this Rummikub AI! Good luck (you'll need it)!\n"
        << "When asked to enter tiles, use the format of <Color Letter><number> in a list separated by spaces\n"
        << "B - black, b - blue, r - red, y - yellow\n";
    
    Board board;
    board.Play();
}