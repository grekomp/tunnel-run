#include "Segment.h"



Segment::Segment()
{
}


Segment::~Segment()
{
}

GameStatus Segment::CheckCollisions(GameStatus status) {
	for (int i = 0; i < obstacles.size(); i++) {
		if (obstacles[i].CheckCollision(status, position) == true) {
			status.gameOver = true;
		}
	}
	
	for (int i = 0; i < pickups.size(); i++) {
		status = pickups[i].CheckCollision(status, position);
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

void Segment::Render(glm::mat4 viewMatrix) {
	for (int i = 0; i < obstacles.size(); i++) {
		obstacles[i].Render(viewMatrix, position);
	}
}

void Segment::RenderPickups(glm::mat4 viewMatrix, GameStatus status) {
	for (int i = 0; i < pickups.size(); i++) {
		pickups[i].Render(viewMatrix, position, status);
	}
}