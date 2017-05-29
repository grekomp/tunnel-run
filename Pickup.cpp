#include "Pickup.h"

Pickup::Pickup()
{
}


Pickup::~Pickup()
{
}

GameStatus Pickup::CheckCollision(GameStatus status) {
	if (!isActive || status.lane != lane) return status;

	if (glm::distance(status.cameraPosition, position) < collisionDistance) {
		isActive = false;
		status.score += score;
	}

	return status;
}

void Pickup::Render(glm::mat4 viewMatrix) {
	if (isActive)
		model.Render(GL_TRIANGLES, viewMatrix);
}

void Pickup::Dispose() {
	model.Dispose();
}