#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <time.h>
#include "Map.h"
class LevelGeneration2D
{
private:
	std::vector<std::vector<char>> grid;
	std::vector<std::vector<char>> gridCopy;
	std::vector< std::vector<std::pair<int, int>>> rooms;
public:
	LevelGeneration2D() {
		srand(time(NULL));
	}

	void setCell(int xCoordinate, int yCoordinate, char setValue);

	void setGrid();

	std::vector<std::vector<char>> getGrid();

	void generation();

	void refine();

	int lifeCheck(const std::array<int, 8>& neighbourState);

	void coutGrid();

	void coutGridNoWalls();

	void coutRooms();

	void coutRoomCount();

	void floodFill();

	Map to_map();

	void floodFillCaverns(int y, int x, char fillNumber);

	std::vector<std::pair<int, int>> enemyGeneration(std::vector<std::pair<int, int>>, int);

	void enemyPlacement();

	Map create_random_map();
};

