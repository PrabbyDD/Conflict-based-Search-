#pragma once
#include <unordered_map>
#include <set>
#include <iostream>
#include "Board.h"
#include <boost/functional/hash.hpp>

struct Position {
    int x,y; // Position of King

    bool isEmpty() const {
        return x == -1 && y == -1;
    }    
    bool operator==(const Position& i) const {
        return x == i.x && y == i.y; 
    }
    
};

struct PositionHash {
    std::size_t operator()(const Position& pos) const {
        return std::hash<int>()(pos.x) ^ std::hash<int>()(pos.y);
    }
};

// This reads that king_id cannot be at position (x,y) at time t
struct Constraint {
    Position position;
    int king_id;
    int time; 
};

// This reads that king1 and king2 have a conflict at position p at time t
struct Conflict {
    int king_id1;
    int king_id2; 
    Position position;
    int time; 

    Conflict(int king1, int king2, Position pos, int t) : king_id1(king1), king_id2(king2), position(pos), time(t) {}
};

// Records when a piece moves so we can output to solution in the correct order
struct Move {
    Position from;
    Position to; 
    int king_id; 
};

class KingsManager {
    private:
        int curr_id = 1; // new King made will be @ curr_id. Will also be displayed on board @ this id
        std::unordered_map<int, Position> kingMap; // Maps ID of spawned king to its current position
        std::unordered_map<int, Position> kingGoalMap; // Maps ID of spawned king to where it needs to be
        std::set<std::pair<int,int>> goals; // DS to help check if goal exists, using pair so dont need to make custom hash, for some reason unordered_set gave issues? 

    public: 
        KingsManager();
        void displayKings(); // Shows where all the kings are, not in order, by iterating through the unordered_map kingMap
        int numKings(); // Returns total number of kings, which is equal to curr_id
        int createKing(int x, int y, int goalX, int goalY, Board& board); // creates a new king and return its ID
        Position getKingPosition(int id); // Returns position of where king is using ID
        Position getKingGoal(int id); // Returns goal position of king using ID
        void move(int id, int dx, int dy, Board& board); // Moves a king by +dx and +dy
        void moveTo(int id, int x, int y, Board& board); // Moves a king to space x,y
        int occupiedByKing(const Position& pos); // Check if space is occupied by a king, and if so which one
};