#include "Enemy.hpp"
#include <algorithm>



Enemy::Enemy(int positionX_, int positionY_)
{	
	PositionX = positionX_;
	PositionY = positionY_;
}


void Enemy::moveAiByPath(std::vector<std::pair<int, int> > pathToWalkBy)
{
	for (int i = 0; i < pathToWalkBy.size(); i++)
	{
		std::cout << pathToWalkBy[i].first << ", " << pathToWalkBy[i].second << std::endl;
		PositionY = pathToWalkBy[i].first;
		PositionX = pathToWalkBy[i].second;
	}

}

std::vector<std::pair<int, int> > Enemy::trackThePath(int startingPositionY, int startingPositionX, int finalPositionY, int finalPositionX, std::vector<std::vector<std::pair<int, int>>>& parentNodes)
{
	std::vector<std::pair<int, int>> path;
	std::pair<int, int> curPos(finalPositionY, finalPositionX);

	while (true)
	{
		path.push_back(curPos);
		if (curPos.first == startingPositionY && curPos.second == startingPositionX)
			break;
		curPos = parentNodes[curPos.first][curPos.second];
	}

	std::reverse(path.begin(), path.end());
	moveAiByPath(path);
	return path;
}

void Enemy::exploreNeighbours(int rowIndex_, int columnIndex_,std::queue <int>& rowQueue , std::queue <int>& columnQueue , int& nodesInNextLayer, std::vector<std::vector<bool> >& visitedNodes, std::vector<std::vector<std::pair<int, int>>>& parentNodes, std::vector<std::vector<char> >& map)
{
	int newRowIndex;
	int newColumnIndex;

	int directionVectorsRows[] = { -1 , +1 , 0 , 0 };
	int direcitonVectorsColumns[] = { 0 , 0 , +1 , -1 };


	for (int i = 0; i < 4; i++)
	{
		newRowIndex = rowIndex_ + directionVectorsRows[i];
		newColumnIndex = columnIndex_ + direcitonVectorsColumns[i];

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
		parentNodes[newRowIndex][newColumnIndex] = std::pair<int, int>(rowIndex_, columnIndex_);
		nodesInNextLayer++;
	}
}

std::vector<std::pair<int, int> > Enemy::findTheWay(int startingPositionY, int startingPositionX, int finalPositionY, int finalPositionX, std::vector<std::vector<char> >& map)
{
	
	int mapRows = map.size();
	int mapColumns = map[0].size();

	std::queue <int> rowQueue;
	std::queue <int> columnQueue;

	int moveCount = 0;
	int nodesLeftInLayer = 1;
	int nodesInNextLayer = 0;

	bool reachedEnd = false;

	std::vector<bool> tempBoolRow(mapColumns, false);
	std::vector<std::vector<bool> > visitedNodes(mapRows, tempBoolRow);

	std::vector<std::pair<int, int>> tempPairRow(mapColumns, std::make_pair(0,0));
	std::vector<std::vector<std::pair<int, int>>> parentNodes(mapRows, tempPairRow);
	
	

	rowQueue.push(startingPositionY);
	columnQueue.push(startingPositionX);

	visitedNodes[startingPositionY][startingPositionX] = true;

	while (!rowQueue.empty())
	{
		int rowIndex = 0, columnIndex = 0;

		if (!rowQueue.empty()) {
			rowIndex = rowQueue.front();
			rowQueue.pop();
		}
		if (!columnQueue.empty()) {
			columnIndex = columnQueue.front();
			columnQueue.pop();
		}

		if (map[rowIndex][columnIndex] == 'F')
		{
			reachedEnd = true;
			break;
		}

		exploreNeighbours(rowIndex, columnIndex, rowQueue, columnQueue , nodesInNextLayer, visitedNodes, parentNodes , map);
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
		return trackThePath(startingPositionY, startingPositionX, finalPositionY, finalPositionX,  parentNodes);
	}
	else {
		return std::vector<std::pair<int, int>>();
	}

}
