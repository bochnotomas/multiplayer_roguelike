#include "LevelGeneration2D.h"

void LevelGeneration2D::setCell(int xCoordinate, int yCoordinate, char setValue) {
	grid[yCoordinate][xCoordinate] = setValue;
}

std::vector<std::vector<char>> LevelGeneration2D::getGrid() {
	return grid;
}

void LevelGeneration2D::setGrid() {
	std::vector<std::vector<char>> firstGeneration;
	for (int y = 0; y <= 99; y++) {
		std::vector<char> temp;
		for (int x = 0; x <= 99; x++) {
			temp.push_back(' ');
		}
		firstGeneration.push_back(temp);
	}
	firstGeneration[49][50] = 'W';
	firstGeneration[51][50] = 'W';
	firstGeneration[52][51] = 'W';

	grid = firstGeneration;
}

void LevelGeneration2D::generation() {
	std::vector<std::vector<char>> secondGeneration;
	for (int i = 0; i <= 99; i++) {
		std::vector<char> tempRow(100, 0);
		secondGeneration.push_back(tempRow);
	}
	int life = 0;
	int liveNeighbours;
	std::array<int, 8> neighbourState = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int y = 0; y <= 99; y++) {
		for (int x = 0; x <= 99; x++) {
			if (y == 0 || y == 99 || x == 0 || x == 99) {
				secondGeneration[y][x] = '#';
				continue;
			}
			neighbourState[0] = grid[y - 1][x - 1];
			neighbourState[1] = grid[y - 1][x];
			neighbourState[2] = grid[y - 1][x + 1];
			neighbourState[3] = grid[y][x - 1];
			neighbourState[4] = grid[y][x + 1];
			neighbourState[5] = grid[y + 1][x - 1];
			neighbourState[6] = grid[y + 1][x];
			neighbourState[7] = grid[y + 1][x + 1];
			liveNeighbours = lifeCheck(neighbourState);
			if (liveNeighbours < 2) {
				secondGeneration[y][x] = ' ';
			}
			else if (liveNeighbours == 2 || liveNeighbours == 3) {
				secondGeneration[y][x] = 'W';
			}
			else if (liveNeighbours > 3) {
				secondGeneration[y][x] = ' ';
			}
		}
	}
	grid = secondGeneration;
}

void LevelGeneration2D::refine() {
	std::vector<std::vector<char>> secondGeneration;
	for (int i = 0; i <= 99; i++) {
		std::vector<char> tempRow(100, 0);
		secondGeneration.push_back(tempRow);
	}
	int life = 0;
	int liveNeighbours;
	std::array<int, 8> neighbourState = { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int y = 0; y <= 99; y++) {
		for (int x = 0; x <= 99; x++) {
			if (y == 0 || y == 99 || x == 0 || x == 99) {
				secondGeneration[y][x] = '#';
				continue;
			}
			neighbourState[0] = grid[y - 1][x - 1];
			neighbourState[1] = grid[y - 1][x];
			neighbourState[2] = grid[y - 1][x + 1];
			neighbourState[3] = grid[y][x - 1];
			neighbourState[4] = grid[y][x + 1];
			neighbourState[5] = grid[y + 1][x - 1];
			neighbourState[6] = grid[y + 1][x];
			neighbourState[7] = grid[y + 1][x + 1];
			liveNeighbours = lifeCheck(neighbourState);
			if ((liveNeighbours < 3) && (grid[y][x] == 'W')) {
				secondGeneration[y][x] = ' ';
			}
			else if ((liveNeighbours > 4) && (grid[y][x] == ' ')) {
				secondGeneration[y][x] = 'W';
			}
			else if (grid[y][x] == ' ') {
				secondGeneration[y][x] = ' ';
			}
			else if (grid[y][x] == 'W') {
				secondGeneration[y][x] = 'W';
			}
		}
	}
	grid = secondGeneration;
}

int LevelGeneration2D::lifeCheck(const std::array<int, 8>& neighbourState) {
	int count = 0;
	for (int i = 0; i <= 7; i++) {
		if (neighbourState[i] == 'W') {
			count++;
		}
	}
	return count;
}

void LevelGeneration2D::coutGrid() {
	for (int y = 0; y <= 99; y++) {
		for (int x = 0; x <= 99; x++) {
			std::cout << grid[y][x];
		}
		std::cout << std::endl;
	}
}

void LevelGeneration2D::coutGridNoWalls() {
	for (int y = 0; y <= 99; y++) {
		for (int x = 0; x <= 99; x++) {
			if (grid[y][x] == 'W') {
				std::cout << ' ';
				continue;
			}
			std::cout << gridCopy[y][x];
		}
		std::cout << std::endl;
	}
}

void LevelGeneration2D::coutRooms() {
	for (int y = 0; y <= (rooms.size() - 1); y++) {
		for (int x = 0; x <= (rooms[y].size() - 1); x++) {
			std::cout << rooms[y][x].first << ',' << rooms[y][x].second;
		}
		std::cout << std::endl;
	}
}

void LevelGeneration2D::coutRoomCount() {
	std::cout << "There are " << rooms.size() << " rooms." << std::endl;
}

void LevelGeneration2D::floodFill() {
	gridCopy = grid;
	int fillNumberInt = 1;
	for (int y = 1; y <= 98; y++) {
		for (int x = 1; x <= 98; x++) {
			if (gridCopy[y][x] == ' ') {
				std::vector<std::pair<int, int>> row;
				row.push_back({ y, x });
				rooms.push_back(row);
				char fillNumber = '0' + fillNumberInt;
				floodFillCaverns(y, x, fillNumber);
				fillNumberInt++;
			}
		}
	}
}

void LevelGeneration2D::floodFillCaverns(int y, int x, char fillNumber) {
	if (gridCopy[y][x] != ' ') {
		return;
	}
	gridCopy[y][x] = fillNumber;
	int fillNumberInt = int(fillNumber - '0');
	rooms[fillNumberInt - 1].push_back(std::pair<int, int>(y, x));

	if (x > 0) {
		floodFillCaverns(y, x - 1, fillNumber);
	}
	if (x < 98) {
		floodFillCaverns(y, x + 1, fillNumber);
	}
	if (y > 0) {
		floodFillCaverns(y - 1, x, fillNumber);
	}
	if (y < 98) {
		floodFillCaverns(y + 1, x, fillNumber);
	}
}