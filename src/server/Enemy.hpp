#ifndef ENEMY_H
#define ENEMY_H
#include <iostream>
#include <queue>
#include "Enemy.hpp"
#include "Player.hpp"

#include <thread>

#include "Map.h"

class Enemy
{
	int PositionX;
	int PositionY;
	std::vector<std::pair<int, int>> currentPath;
	

	Enemy(int positionX_, int positionY_);
	
	void moveAiByPath(std::vector<std::pair<int, int> > pathToWalkBy);
	
	std::vector<std::pair<int, int> > trackThePath(int startingPositionY, int startingPositionX, int finalPositionY, int finalPositionX, std::vector<std::vector<std::pair<int, int>>>& parentNodes);
	
	void exploreNeighbours(int rowIndex_, int columnIndex_, std::queue <int>& rowQueue, std::queue <int>& columnQueue, int& nodesInNextLayer, std::vector<std::vector<bool> >& visitedNodes, std::vector<std::vector<std::pair<int, int>>>& parentNodes, Map& map);
	
	std::vector<std::pair<int, int> > findTheWay(int startingPositionY, int startingPositionX,int finalPositionY, int finalPositionX , Map& map);

	void aiTick(const std::vector<std::shared_ptr<Player> >& players, Map& map);
};

#endif 
