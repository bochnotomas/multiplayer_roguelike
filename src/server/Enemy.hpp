#ifndef ENEMY_H
#define ENEMY_H
#include <iostream>
#include <queue>
#include "Enemy.hpp"
#include "Player.hpp"

#include <thread>
#include "Object.h"

#include "Map.h"

class Enemy : public Object
{
	std::vector<std::pair<int, int>> currentPath;

	// old function which made enemy to teleport to final destination, wrong
	//void moveAiByPath(std::vector<std::pair<int, int> > pathToWalkBy);
	
	//function that return vector of pairs, that represent the path
	std::vector<std::pair<int, int> > trackThePath(int startingPositionY, int startingPositionX, int finalPositionY, int finalPositionX, std::vector<std::vector<std::pair<int, int>>>& parentNodes);

	//function that explore neighbours of node
	void exploreNeighbours(int rowIndex_, int columnIndex_, std::queue <int>& rowQueue, std::queue <int>& columnQueue, int& nodesInNextLayer, std::vector<std::vector<bool> >& visitedNodes, std::vector<std::vector<std::pair<int, int>>>& parentNodes, Map& map);

	//function that use bfs to find the shortest path to either, final destination or player
	std::vector<std::pair<int, int> > findTheWay(int startingPositionY, int startingPositionX,int finalPositionY, int finalPositionX , Map& map);

public:
	Enemy(int positionX_, int positionY_);
	//function that makes enemy move by one step every turn
	//it either chase an player, or randomise a position and go there
	void aiTick(const std::vector<std::shared_ptr<Player> >& players, Map& map);
};

#endif 
