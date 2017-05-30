#include "Game.h"


Game::Game()
{
}


Game::~Game()
{
}


void Game::NextFrame() {
	std::chrono::time_point<std::chrono::system_clock> now;
	now = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsed_seconds = now - lastFrame;

	float deltaTime = elapsed_seconds.count();

	status.time += deltaTime;
	status.deltaTime = deltaTime;

	status.cameraPosition.z -= cameraSpeedZ * deltaTime;

	for (int i = 0; i < segments.size(); i++) {
		status = segments[i].CheckCollisions(status);

		if (segments[i].position.z - segments[i].length > status.cameraPosition.z) {
			segments[i].Dispose();
			segments.erase(segments.begin() + i);
		}
	}

	/*
	if (glm::distance(segments[segments.size() - 1].position, status.cameraPosition) > renderDistance) {
		Segment newSegment;

		newSegment.position = segments[segments.size() - 1].position;
		newSegment.position.z - segments[segments.size() - 1].length;


		segments.push_back(newSegment);
	}*/

	if (status.cameraPosition.z - corridorSegments[corridorSegments.size() - 1].position.z - corridorSegments[corridorSegments.size() - 1].length > renderDistance) {

	}

	if (status.gameOver) exit(69);

	lastFrame = now;
}

void Game::Render() {

	// Creating the view matrix
	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(0, 0, 10),
		glm::vec3(0, 0, -1),
		//glm::vec3(status.cameraPosition + glm::vec4(0.0, 0.0, -1.0, 0.0)),
		glm::vec3(0, 1, 0)
	);

	// Sending view matrix
	glUniformMatrix4fv(VIEW_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Recalculating light position to eye coordinates
	glm::vec4 lightPos = viewMatrix * status.cameraPosition;
	glUniform4fv(LIGHT_POSITION_LOCATION, 1, glm::value_ptr(lightPos));

	// Rendering all objects
	for (int i = 0; i < segments.size(); i++) {
		segments[i].Render(viewMatrix);
	}

	// Rendering corridor
	for (int i = 0; i < corridorSegments.size(); i++) {
		corridorSegments[i].Render(viewMatrix);
	}
}