#pragma once
#include <vector>

// I realized these functions should all be static similarly in KingsManager but I dont want to change them all this late and break something
class Board {
    private:
        std::vector<std::vector<int>> board; 
    public:
        Board();
        void populateBoard(int n);
        int get(int x, int y) const; // Get what lives at this position on the board
        int getBoardSize() const; // Gets board size
        void change(int x, int y, int c); // Changes what is at this cell
        void moveObject(int x_from, int y_from, int x_to, int y_to); // Moves object from one spot to another using change()
        void display() const; // Outputs the board to stdout
        void addObstacle(int x, int y); // Wrapper for change specifically for new obstacles
        bool alreadyOccupied(int x, int y) const; // Tells us if a space is already occupied by something else
        

};