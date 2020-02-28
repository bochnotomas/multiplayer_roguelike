#pragma once
#include <array>
#include <vector>
#include <iostream>
class LevelGeneration2D
{
private:
	std::vector<std::vector<char>> grid;
	std::vector<std::vector<char>> gridCopy;
	std::vector< std::vector<std::pair<int, int>>> rooms;
public:

	void setCell(int xCoordinate, int yCoordinate, char setValue);

	void setGrid();

	void generation();

	void refine();

	int lifeCheck(const std::array<int, 8>& neighbourState);

	void coutGrid();

	void coutGridNoWalls();

	void coutRooms();

	void coutRoomCount();

	void floodFill();

	void floodFillCaverns(int y, int x, char fillNumber);
};

