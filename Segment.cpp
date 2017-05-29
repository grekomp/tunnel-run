#include "Segment.h"



Segment::Segment()
{
}


Segment::~Segment()
{
}

GameStatus Segment::CheckCollisions(GameStatus status) {
	for (int i = 0; i < obstacles.size(); i++) {
		if (obstacles[i].CheckCollision(status)) {
			status.gameOver = true;
		}
	}
	
	for (int i = 0; i < pickups.size(); i++) {
		status = pickups[i].CheckCollision(status);
	}

	return status;
}

void Segment::Dispose() {
	for (int i = 0; i < obstacles.size(); i++) {
		obstacles[i].Dispose();
	}

	for (int i = 0; i < pickups.size(); i++) {
		pickups[i].Dispose();
	}
}