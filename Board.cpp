#include "Board.h"
#include <iostream>

// Empty constructor
Board::Board() {}

void Board::populateBoard(int n) {
    board = std::vector<std::vector<int>>(n, std::vector<int>(n, 0)); 
}

void Board::addObstacle(int x, int y) {
    change(x,y,-1); 
}

int Board::getBoardSize() const {
    return board.size(); 
}

void Board::display() const {
    
    for (int i = 0; i < getBoardSize()*2; i++) {
        std::cout << "-";
    }
    std::cout << std::endl; 


    for (auto& r: board) {
        for (int c: r) {
            std::cout << c << " ";
        }
        std::cout << "\n";
    }

    for (int i = 0; i < getBoardSize()*2; i++) {
        std::cout << "-";
    }
    std::cout << std::endl; 
}

bool Board::alreadyOccupied(int x, int y) const {
    if (board[y][x] == 0) {
        return 0;
    } else {
        return 1; 
    }
}

void Board::moveObject(int x_from, int y_from, int x_to, int y_to) {
    if (alreadyOccupied(x_to, y_to)) {
        std::cerr << "Position already occupied: " << x_to << ", " << y_to << std::endl; 
        std::cerr << "It is: " << get(x_to, y_to) << std::endl; 
        return; 
    }
    int temp = get(x_from, y_from); 
    change(x_to, y_to, temp);
    change(x_from, y_from, 0); 
}

int Board::get(int x, int y) const {
    return board[y][x]; 
}

void Board::change(int x, int y, int c) {
    board[y][x] = c; 
}

