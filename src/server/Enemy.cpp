#include <iostream>
#include <queue>
#include "../client/Map.h"

class Enemy
{
public:
	int startingPositionX;
	int startingPositionY;
	
	//variables for BFS
	int mapRows;
	int mapColumns;

	char matrix[mapRows][mapColumns];
	char map[mapRows][mapColumns];


	int finalPositionX = 2;
	int finalPositionY = 4;

	std::queue <int> rowQueue;
	std::queue <int> columnQueue;

	int moveCount = 0;
	int nodesLeftInLayer = 1;
	int nodesInNextLayer = 0;

	bool reachedEnd = false;

	bool visitedNodes[mapRows - 2][mapColumns - 2];

	int directionVectorsRows[] = { -1 , +1 , 0 , 0 };
	int direcitonVecotrsColumns[] = { 0 , 0 , +1 , -1 };

	std::vector <int> parentNodesRows;
	std::vector <int> parentNodesColumns;

	Enemy(int positionX_, int positionY_, Map& mapObject )
	{
		startingPositionX = positionX_;
		startingPositionY = positionY_;	
		mapRows = mapObject.get_map_size().second;
		mapColumns = mapObject.get_map_size().first;
	}

	void outputPreviousVector()
	{
		std::cout << "Rows: ";
		for (int i = 0; i < parentNodesRows.size(); i++)
		{
			std::cout << parentNodesRows[i] << ", ";
		}
		std::cout << std::endl;

		std::cout << "Columns: ";
		for (int i = 0; i < parentNodesColumns.size(); i++)
		{
			std::cout << parentNodesColumns[i] << ", ";
		}

		std::cout << std::endl;
	}

	void exploreNeighbours(int rowIndex_, int columnIndex_)
	{
		int newRowIndex;
		int newColumnIndex;

		parentNodesRows.push_back(rowIndex_);
		parentNodesColumns.push_back(columnIndex_);

		for (int i = 0; i < 4; i++)
		{
			newRowIndex = rowIndex_ + directionVectorsRows[i];
			newColumnIndex = columnIndex_ + direcitonVecotrsColumns[i];

			if (newRowIndex < 1 || newColumnIndex < 1)
			{
				continue;
			}
			if (newRowIndex > 8 || newColumnIndex > 8)
			{
				continue;
			}

			if (visitedNodes[newRowIndex][newColumnIndex])
			{
				continue;
			}
			if (map[newRowIndex][newColumnIndex] == '#')
			{
				continue;
			}

			rowQueue.push(newRowIndex);
			columnQueue.push(newColumnIndex);


			visitedNodes[newRowIndex][newColumnIndex] = true;
			nodesInNextLayer++;
		}
	}

	int findTheWay(int startingPositionY, int startingPositionX)
	{

		rowQueue.push(startingPositionY);
		columnQueue.push(startingPositionX);

		visitedNodes[startingPositionY][startingPositionX] = true;



		while (rowQueue.size() > 0)
		{
			int rowIndex = rowQueue.front(); rowQueue.pop();
			int columnIndex = columnQueue.front(); columnQueue.pop();

			if (map[rowIndex][columnIndex] == 'F')
			{
				reachedEnd = true;
				break;
			}

			exploreNeighbours(rowIndex, columnIndex);
			nodesLeftInLayer--;
			if (nodesLeftInLayer == 0)
			{
				nodesLeftInLayer = nodesInNextLayer;
				nodesInNextLayer = 0;
				moveCount++;
			}
		}
		if (reachedEnd == true)
		{
			return moveCount;
		}
		else {
			return -1;
		}

	}
};