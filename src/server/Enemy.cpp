#include "Enemy.hpp"
#include <algorithm>
#include <cmath>





Enemy::Enemy(int positionX_, int positionY_) : Object('X', Direction::NORTH, true, {positionX_, positionY_})
{	

}


void Enemy::aiTick(const std::vector<std::shared_ptr<Player> >& players, Map& map)
{
	std::shared_ptr<Player> chasing = nullptr;

	if (chasing != nullptr)
	{
		float distance = sqrt(pow((m_position.first - chasing->playerPositionX), 2) + pow((m_position.second - chasing->playerPositionY), 2));
		if(distance > 5)
		{
			chasing = nullptr;
			return;
		}
		currentPath = findTheWay(m_position.second, m_position.first, chasing->playerPositionY, chasing->playerPositionX, map);
		m_position.first = currentPath[0].first;
		m_position.second = currentPath[0].second;
		currentPath.erase(currentPath.begin());
	}
	else {
		float closestDistance = 5;
		for(int i = 0 ; i < players.size(); i++)
		{
			float distance = sqrt(pow((m_position.first - players[i]->playerPositionX), 2) + pow((m_position.second - players[i]->playerPositionY), 2));
			if(distance <= closestDistance)
			{
				closestDistance = distance;
				chasing = players[i];
			}
		}

		if(chasing != nullptr)
		{
			currentPath = findTheWay(m_position.second, m_position.first, chasing->playerPositionY, chasing->playerPositionX, map);
			m_position.first = currentPath[0].first;
			m_position.second = currentPath[0].second;
			currentPath.erase(currentPath.begin());
		}else
		{
			if(currentPath.empty())
			{
				int randomFinalPositionX;
				int randomFinalPositionY;
				while(true )
				{
					randomFinalPositionX = rand() % map.get_map_size().first;
					randomFinalPositionY = rand() % map.get_map_size().second;
					if ((*map.get_map_plane())[randomFinalPositionX][randomFinalPositionY].accesible)
						break;

				}
				currentPath=findTheWay(m_position.second, m_position.first, randomFinalPositionY, randomFinalPositionX, map);
			}

			if (!currentPath.empty()) {
				m_position.first = currentPath[0].first;
				m_position.second = currentPath[0].second;
				currentPath.erase(currentPath.begin());
			}
		}
	}
}
/*
void Enemy::moveAiByPath(std::vector<std::pair<int, int> > pathToWalkBy)
{
	for (int i = 0; i < pathToWalkBy.size(); i++)
	{
		std::cout << pathToWalkBy[i].first << ", " << pathToWalkBy[i].second << std::endl;
		m_position.second = pathToWalkBy[i].first;
		m_position.first = pathToWalkBy[i].second;
	}

}
*/
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
	//moveAiByPath(path);
	return path;
}

void Enemy::exploreNeighbours(int rowIndex_, int columnIndex_,std::queue <int>& rowQueue , std::queue <int>& columnQueue , int& nodesInNextLayer, std::vector<std::vector<bool> >& visitedNodes, std::vector<std::vector<std::pair<int, int>>>& parentNodes, Map& map)
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
		if (!(*map.get_map_plane())[newColumnIndex][newRowIndex].accesible)
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

std::vector<std::pair<int, int> > Enemy::findTheWay(int startingPositionY, int startingPositionX, int finalPositionY, int finalPositionX, Map& map)
{
	
	int mapRows = map.get_map_size().second;
	int mapColumns = map.get_map_size().first;

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

		if (rowIndex == finalPositionY && columnIndex == finalPositionX)
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
