#include "Pickup.h"

Pickup::Pickup()
{
}


Pickup::~Pickup()
{
}

bool Pickup::CheckCollision(GameStatus status) {
	if (status.lane != lane) return lane;

	if (glm::distance(status.cameraPosition, position) < collisionDistance) return true;

	return false;
}

void Pickup::Render(glm::mat4 viewMatrix) {
	model.Render(GL_TRIANGLES, viewMatrix);
}