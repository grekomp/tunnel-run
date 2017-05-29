#include "Obstacle.h"

Obstacle::Obstacle()
{
}


Obstacle::~Obstacle()
{
}

bool Obstacle::CheckCollision(GameStatus status) {
	if (status.lane != lane) return false;

	if (glm::distance(status.cameraPosition, position) < collisionDistance) return true;

	return false;
}

void Obstacle::Render(glm::mat4 viewMatrix) {
	model.Render(GL_TRIANGLES, viewMatrix);
}