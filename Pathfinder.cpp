#include "Pathfinder.h"
#include <unordered_set>
#include <queue>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/functional/hash.hpp>

// In a grid we can use the manhatten distance typically
int Pathfinder::heuristic(const Position& a, const Position& b) {
    return  std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Position> Pathfinder::aStarPath(KingsManager& kings, std::vector<Constraint>& constraints, int king_id, Board& board, std::vector<Move>& moves) {
    // Part 1 of A*: setting up PQ and visited DS, and placing starting point in the PQ
    std::priority_queue<GraphNode> PQ; // Should be minPQ, we want the smaller values popped off first, comparator made in GraphNode declaration
    std::unordered_set<Position, PositionHash> visited; 

    Position curr_king_pos = kings.getKingPosition(king_id);
    Position curr_king_goal = kings.getKingGoal(king_id); 

    // Error check for proper king ID
    if (curr_king_pos.isEmpty() || curr_king_goal.isEmpty()) {
        return {}; 
    }
    
    PQ.emplace(curr_king_pos, nullptr, 0, heuristic(curr_king_pos, curr_king_goal)); 

    while (!PQ.empty()) {

        // 3rd part of A*, popping off top of PQ and checking if we are at the goal
        GraphNode cur = PQ.top();
        PQ.pop(); 

        int cur_x = cur.position.x;
        int cur_y = cur.position.y; 

        // Check if we are at the goal, if so return the path we took to get here
        if (cur_x == curr_king_goal.x && cur_y == curr_king_goal.y) {
            std::vector<Position> path;
    
            // Trace back each parent and put in the path till we reach starting position GraphNode/King
            for (GraphNode* n = &cur; n != nullptr; n = n->parent) {
                path.push_back(n->position); 
            }
            std::reverse(path.begin(), path.end());

            // For outputting correct sequence of moves to solution.txt
            Position start = kings.getKingPosition(king_id); 
            for (size_t i = 1; i < path.size(); i++) {
                
                moves.push_back({path[i - 1], path[i], king_id}); 
                // Update board 
                kings.moveTo(king_id, path[i].x, path[i].y, board); 
            }

            std::cout << "King " << king_id << " found a path from (" << start.x << ", " << start.y << ") to (" << kings.getKingGoal(king_id).x << ", " << kings.getKingGoal(king_id).y << ")" << std::endl; 
            return path; 
        }

        // 2nd part of A*, checking all your neighbors and ordering them in the PQ by closest to goal
        // If already visited this node, skip, else add it to visited set and then check neighbors
        if (visited.count(cur.position)) continue;
        visited.insert(cur.position); 

        // 8 directions of neighbors
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // If we are at current position go to next
                if (i == 0 && j == 0) continue; 
                Position neighbor = {cur.position.x + i, cur.position.y + j}; 

                // OOB checking and checking if board space is empty and check if current king is not constrained at the neighbor position or space occupied
                if (neighbor.x >= 0 && neighbor.x < board.getBoardSize() &&
                    neighbor.y >= 0 && neighbor.y < board.getBoardSize() && 
                    visited.count(neighbor) == 0 && 
                    !constrained(king_id, cur.g + 1, neighbor, constraints) &&
                    !board.alreadyOccupied(neighbor.x, neighbor.y) && 
                    !kings.occupiedByKing(cur.position)) {
                        GraphNode new_neighbor_node(neighbor, new GraphNode(cur), cur.g + 1, heuristic(neighbor, curr_king_goal)); 
                        PQ.push(new_neighbor_node); 
                }
            }
        }
    }

    // Return nothing is path not viable
    std::cerr << "No viable path plan found!" << std::endl; 
    return {};
}

std::vector<std::vector<Position>> Pathfinder::allStarPath(KingsManager& kings, std::vector<Constraint>& constraints, Board& board, std::vector<Move>& moves) {
    std::vector<std::vector<Position>> paths; 
    for (int id = 1; id < kings.numKings() + 1; id++) {
        paths.push_back(aStarPath(kings, constraints, id, board, moves));
    }
    return paths; 
}

void Pathfinder::updateBoardAndDisplay(std::vector<Position> path, Board& board, int idx) {
    // OOB check
    if (idx - 1 >= path.size()) {
        std::cerr << "Given index for updateBoard: " << idx << " is OOB for given path of size: " << path.size() << std::endl;
        return; 
    }
    
    // initial board before movement
    board.display();

    for (int i = 0; i < idx - 1; i++) {
        board.moveObject(path[i].x, path[i].y, path[i + 1].x, path[i + 1].y);
        board.display(); 
    }
}  

void Pathfinder::updateBoard(std::vector<Position> path, Board& board, int idx) {
    // OOB check
    if (idx - 1 >= path.size()) {
        std::cerr << "Given index for updateBoard: " << idx << " is OOB for given path of size: " << path.size() << std::endl;
        return; 
    }
    
    for (int i = 0; i < idx - 1; i++) {
        board.moveObject(path[i].x, path[i].y, path[i + 1].x, path[i + 1].y);
    }
}   

bool Pathfinder::constrained(int king_id, int time, Position p, std::vector<Constraint>& constraints) {
    for (const Constraint& c: constraints) {
        if (c.king_id == king_id && c.position == p && c.time == time) {
            return true;
        }
    }
    return false; 
}

// For the hash function for boost in findConflicts function
std::size_t hash_value(const Position& pos) {
    std::size_t seed = 0;
    boost::hash_combine(seed, pos.x);
    boost::hash_combine(seed, pos.y);
    return seed;
}

Conflict* Pathfinder::findConflicts(const std::unordered_map<int, std::vector<Position>>& paths) {
    std::unordered_map<std::pair<Position,int>, int, boost::hash<std::pair<Position, int>>> occupancyMap; // Maps [position, time] -> king_id step

    for (auto& [king_id, path] : paths) {
        for (int time = 0; time < path.size(); time++) {
            auto key = std::make_pair(path[time], time); 
            // If we find this spot is already taken at this time, we have a conflict
            if (occupancyMap.find(key) != occupancyMap.end()) {
                int conflicting_king = occupancyMap[key];
                Conflict* conflict = new Conflict(king_id, conflicting_king, path[time],  time); 
                return conflict;
            } else {
                // No conflict yet, add this occupied space to the map
                occupancyMap[key] = king_id; 
            }
        }
    }
    return nullptr; 
}

std::vector<Constraint> Pathfinder::createConstraint(const Conflict& conflicts) {
    return {
        {conflicts.position, conflicts.king_id1, conflicts.time},
        {conflicts.position, conflicts.king_id2,  conflicts.time}
    };
}

int Pathfinder::calcPathCost(const std::vector<Position>& pos) {
    return pos.size(); 
}

int Pathfinder::calcTotalCost(const std::unordered_map<int, std::vector<Position>>& paths) {
    int res = 0;
    for (const auto& [king_id, pathMap] : paths) {
        res += calcPathCost(pathMap); 
    }
    return res; 
}

// The return of this function is the set of all paths for each king that are conflict free, if they exist
std::vector<std::vector<Position>> Pathfinder::CBS(KingsManager& kings, Board& board, std::vector<Move>& moves) {
    std::priority_queue<ConstraintTreeNode> nodeQueue; // PQ that picks the lowest cost constraint tree node -> cost is total number of moves all agents have to make to reach goals
    ConstraintTreeNode root; // First node in our CBS tree

    // Since we are never removing kings from the board, if we have 10 kings, then IDs are 1,2,3...10
    for (int id = 1; id < kings.numKings(); id++) {
        root.pathMap[id] = aStarPath(kings, root.constraints, id, board, moves);  // get path given from A* for this king
    }
    root.cost = calcTotalCost(root.pathMap);
    nodeQueue.push(root); 
    
    // If we dont return in this loop then no solution exists
    while(!nodeQueue.empty()) {

        ConstraintTreeNode currNode = nodeQueue.top();
        nodeQueue.pop(); 
        
        // Check if any conflicts exist, if not then we have a plan that works! 
        auto conflict = findConflicts(currNode.pathMap); 
        if (conflict == nullptr) {
            
            std::vector<std::vector<Position>> answer;
            for (int id = 0; id <= kings.numKings(); id++) {
                answer.push_back(currNode.pathMap[id]); 
            }
            return answer; 
        }

        // A conflict was found, so we must split the current node into 2 nodes, for each pair of conflicting king paths
        std::vector<Constraint> constraints = createConstraint(*conflict); 

        // Go through each constraint and create new nodes that apply the new constraints from the conflicting kings paths
        for (const auto& constraint : constraints) {
            ConstraintTreeNode child = currNode; 
            child.constraints.push_back(constraint); 
            
            // Recompute A* considering new constraints 
            child.pathMap[constraint.king_id] = {aStarPath(kings, child.constraints, constraint.king_id, board, moves)}; 

            // Now we confirm if the new path using updated constraints is valid and add to queue to be popped off if cost is low enough
            // If its not, we need to recompute this path
            if (!child.pathMap[constraint.king_id].empty()) {
                child.cost = calcTotalCost(child.pathMap); 
                nodeQueue.push(child); 
            }
        }
        delete conflict; // free
    }

    // Not possible
    std::cerr << "Impossible to find path that satisfies input!" << std::endl; 
    return {}; 
}
