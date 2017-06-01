#include "Pickup.h"

Pickup::Pickup()
{
}


Pickup::~Pickup()
{
}

GameStatus Pickup::CheckCollision(GameStatus status, glm::vec4 segmentPosition) {
	if (!isActive) return status;

	position = segmentPosition;
	position.x = lane - 2;
	position.y = status.ballPosition.y;

	if (glm::distance(status.ballPosition, position) < collisionDistance + status.ballRadius) {
		isActive = false;
		status.score += score;
	}

	return status;
}

void Pickup::Render(glm::mat4 viewMatrix, glm::vec4 segmentPosition, GameStatus status) {
	if (isActive) {
		model.position = segmentPosition;
		model.position.x = lane - 2;
		model.position.y += glm::cos(status.time * 5) * 0.1f;

		model.localRotation.y = std::fmod(status.time, rotationSpeed) / rotationSpeed * 360.0f;

		glUniform1i(CUBE_SAMPLER_LOCATION, 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		model.Render(GL_TRIANGLES, viewMatrix);
	}
}

void Pickup::Dispose() {
	model.Dispose();
}