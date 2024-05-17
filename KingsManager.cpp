#include "KingsManager.h"

// Empty Constructor
KingsManager::KingsManager() {}

int KingsManager::createKing(int x, int y, int goalX, int goalY, Board& board) {

    if (board.alreadyOccupied(x, y)) {
        std::cerr << "Unsuccessfully tried to create king at occupied space: " << x << ", " << y << std::endl;
        std::cerr << "Occupied by: " << board.get(x,y) << std::endl; 
        std::cerr << "Shutting down planner...." << std::endl;
        return -2; 
    } else {

        kingMap[curr_id] = {x, y};

        //If goal already exists
        if (goals.count({goalX, goalY})) {
            std::cerr << "Unsuccessfully tried to create king with same goal as previously created king: " << x << ", " << y << std::endl;
            std::cerr << "Shutting down planner...." << std::endl;
            return -2; 
        }

        // Goal space is on obstacle, impossible to finish
        if (board.alreadyOccupied(goalX, goalY)) {
            std::cerr << "Unsuccessfully tried to create king with goal on top of obstacle: " << goalX << ", " << goalY << std::endl;
            std::cerr << "Shutting down planner...." << std::endl;
            return -2;
        }

        kingGoalMap[curr_id] = {goalX, goalY}; 
        goals.insert({goalX, goalY}); 


        // Also place king on board here, kinda nasty from OOP perspective but it works, would like to clean this up
        board.change(x, y, curr_id); 
        curr_id++; 
        return curr_id - 1; 
    }
}

void KingsManager::displayKings() {
    for (const auto& [king_id, pathMap] : kingMap) {
        std::cout << "King: " << king_id << ", Position: " << pathMap.x << ", " << pathMap.y << std::endl;  
    }
}

int KingsManager::numKings() {
    return curr_id; 
}

Position KingsManager::getKingPosition(int id) {
    if (!kingMap.count(id)) {
        std::cerr << "Either ID is invalid or king does not exist for ID: " << id << std::endl;
        return {-1,-1}; 
    } else {
        return kingMap[id];
    }
}

Position KingsManager::getKingGoal(int id) {
    if (!kingGoalMap.count(id)) {
        std::cerr << "Either ID is invalid or king does not exist for ID: " << id << std::endl;
        return {-1,-1}; 
    } else {
        return kingGoalMap[id]; 
    }
}

void KingsManager::move(int input_id, int dx, int dy, Board& board) {

    if (board.alreadyOccupied(kingMap[input_id].x + dx, kingMap[input_id].y + dy)) {
        std::cerr << "King tried to move to occupied space: " << kingMap[input_id].x + dx << ", " << kingMap[input_id].y + dy << std::endl;
    } else {
        // Set this spot to 0 before moving
        board.change(kingMap[input_id].x, kingMap[input_id].y, 0);
        kingMap[input_id].x += dx;
        kingMap[input_id].y += dy;
        board.change(kingMap[input_id].x, kingMap[input_id].y, input_id);
    }
}

void KingsManager::moveTo(int id, int x, int y, Board& board) {
    if (board.alreadyOccupied(x, y)) {
        std::cerr << "King tried to move to occupied space: " << x << ", " << y << std::endl;
        return; 
    } else {
        board.change(kingMap[id].x, kingMap[id].y, 0);
        kingMap[id].x = x;
        kingMap[id].y = y; 
        board.change(kingMap[id].x, kingMap[id].y, id);
    }
}

int KingsManager::occupiedByKing(const Position& pos) {
    for (const auto& [king_id, p] : kingMap) {
        if (pos == p) {
            return king_id; 
        }
    }
    return 0; 
}

// Move the moving of a king into this class?
