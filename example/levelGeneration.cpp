#include "../src/server/LevelGeneration2D.h"

int main() {

	LevelGeneration2D one;

	one.setGrid();

	one.coutGrid();

	for (int i = 0; i <= 100; i++) {
		one.generation();
	}

	one.coutGrid();

	for (int i = 0; i <= 3; i++) {
		one.refine();
	}

	one.coutGrid();

	one.floodFill();

	one.coutGridNoWalls();

	one.coutRoomCount();

}