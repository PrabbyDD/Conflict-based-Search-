#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector> 
#include <chrono>

#include "Pathfinder.h"
#include "Board.h"
#include "KingsManager.h"

int main(int kwargs, char** args) {

    if (kwargs != 3) {
        std::cerr << "Incorrect number of input arguments!" << std::endl; 
        return -1; 
    }

    // 2 args passed to the program are "map.txt kings.txt"
    std::ifstream map_file(args[1]);
    if (!map_file.is_open()) {
        std::cerr << "Error opening map file!" << std::endl; 
        return -1; 
    }

    // Read in the map file and make board, could use rdbuf() instead
    Board board; 
    std::string map_size_string;
    if (std::getline(map_file, map_size_string)) {
        std::cout << "Size of map is: " << map_size_string << std::endl; 
        int size = std::stoi(map_size_string); 
        board.populateBoard(size); 
    } else {
        std::cerr << "Error getting map size!" << std::endl; 
        return -1;
    }

    // Read in obstacle positions and populate
    std::string obstacle_string; 
    while(getline(map_file, obstacle_string)) {
        std::istringstream iss(obstacle_string); 
        std::string part; 
        int x, y; 
        if (!getline(iss, part, ',')) continue; // Skip empty lines or no comma lines
        x = std::stoi(part); 

        if (!getline(iss, part)) continue; 
        y = std::stoi(part); 

        board.addObstacle(x,y); 
        std::cout << "Added obstacle at: " << x << ", " << y << std::endl; 
    }

    map_file.close();

    // Read in kings file and populate all kings
    std::ifstream kings_file(args[2]);
    if (!kings_file.is_open()) {
        std::cerr << "Error opening kings file!" << std::endl; 
        return -1; 
    }

    std::string kings_string;
    KingsManager kings;  
    while(getline(kings_file, kings_string)) {
        std::istringstream iss(kings_string); 
        std::string part; 
        int x, y, goalX, goalY; 
        if (getline(iss, part, ',')) {
            x = std::stoi(part); 
        }
        if (getline(iss, part, ',')) {
            y = std::stoi(part); 
        }
        if (getline(iss, part, ',')) {
            goalX = std::stoi(part); 
        }
        if (getline(iss, part, ',')) {
            goalY = std::stoi(part); 
        }
        int code = kings.createKing(x, y, goalX, goalY, board); 

        // Unsucessfully tried to make king
        if (code < 0) {
            return -1; 
        }
        std::cout << "Added King at: " << x << ", " << y << std::endl; 
        std::cout << "Going to: " << goalX << ", " << goalY << std::endl;
    }
    kings_file.close(); 
        
    // Show the kings
    //kings.displayKings(); 

    // display board
    board.display();

    //std::vector<Position> path = Pathfinder::aStarPath(kings, 1, board);

    // for (Position& pos: path) {
    //     std::cout << "King 1 went through: " << pos.x << ", " << pos.y << std::endl; 
    // }
    // Pathfinder::updateBoardAndDisplay(path,board, 4); 
    
    std::vector<Move> moves; 
    auto start = std::chrono::high_resolution_clock::now(); 
    std::vector<std::vector<Position>> results = Pathfinder::CBS(kings, board, moves);
    auto end = std::chrono::high_resolution_clock::now(); 

    std::chrono::duration<double> time = end - start; 
    std::cout << "CBS took: " << time.count() << " seconds." << std::endl; 

    // Output to solution.txt
    if (results.empty()) {
        std::cerr << "No solution found" << std::endl; 
        return -1;
    } else {
        std::ofstream outFile("solution.txt"); 
        for (const auto& move : moves) {
            outFile << move.from.x << ", " << move.from.y << ", " << move.to.x << ", " << move.to.y << "\n";
        }
        outFile.close(); 
    }

    


    return 1; 
}