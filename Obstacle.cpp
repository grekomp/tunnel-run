#include "Obstacle.h"

Obstacle::Obstacle()
{
}


Obstacle::~Obstacle()
{
}

bool Obstacle::CheckCollision(GameStatus status, glm::vec4 segmentPosition) {
	position = segmentPosition;
	position.x = lane - 2;
	position.y = status.ballPosition.y;

	if (glm::distance(status.ballPosition, position) < collisionDistance + status.ballRadius) 
		return true;

	return false;
}

void Obstacle::Render(glm::mat4 viewMatrix, glm::vec4 segmentPosition) {
	model.position = segmentPosition;
	model.position.x = lane - 2;
	model.Render(GL_TRIANGLES, viewMatrix);
}

void Obstacle::Dispose() {
	model.Dispose();
}