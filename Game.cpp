#include "Game.h"

Game::Game()
{
	Setup();
}


Game::~Game()
{
}

void Game::Setup() {
	// Set options
	cameraSpeedZ = 7.0f;
	cameraSpeedX = cameraSpeedZ / speedRatio;
	
	// Clean corridors, segments
	segments.clear();
	corridorSegments.clear();

	// Setup status
	GameStatus st;
	status = st;
	status.cameraPosition = glm::vec4(0.0, 1.0, 10.0, 1.0);
	status.lane = 2;
	status.ballRadius = 0.3f;
	status.ballPosition.y = status.ballRadius;
	status.ballPosition.w = 1.0f;
	status.time = 0.0f;
	status.score = 0;
	status.renderStart = 1.5f;

	// Setup base last frame time
	lastFrame = std::chrono::system_clock::now();
}

void Game::PostSetup() {
	// Adding first corridor piece
	corridorSegments.push_back(corridorBase);

	// Adding first segment
	segments.push_back(baseSegments[0]);

	// Setting last frame time
	lastFrame = std::chrono::system_clock::now();
}

void Game::NextFrame() {
	if (!status.gameOver) {
		// Calculating deltaTime
		std::chrono::time_point<std::chrono::system_clock> now;
		now = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed_seconds = now - lastFrame;

		float deltaTime = elapsed_seconds.count();

		status.time += deltaTime;
		status.deltaTime = deltaTime;

		// Changing camera speed
		if (cameraSpeedZ < maxCameraSpeedZ) {
			cameraSpeedZ += cameraSpeedModifierZ * deltaTime;
			cameraSpeedX = cameraSpeedZ / speedRatio;
		}
		else {
			cameraSpeedZ = maxCameraSpeedZ;
		}

		// Moving camera forward
		status.cameraPosition.z -= cameraSpeedZ * deltaTime;

		// Moving camera horizontally
		status.cameraPosition.x = (float)(status.lane - 2) * cameraSpeedX + status.cameraPosition.x * (1 - cameraSpeedX);

		// Moving ball
		status.ballPosition.x = status.cameraPosition.x;
		status.ballPosition.z = status.cameraPosition.z - ballOffset;

		ball.position = status.ballPosition;

		ball.localRotation.x -= cameraSpeedZ * deltaTime * 360.0f / 2 * glm::pi<float>() * status.ballRadius;
		ball.localRotation.z = ball.position.x * 360.0f / 2 * glm::pi<float>() * status.ballRadius;

		// Checking for collisions
		for (int i = 0; i < segments.size(); i++) {
			status = segments[i].CheckCollisions(status);
		}

		// Removing segments
		if (segments[0].position.z - segments[0].length > status.cameraPosition.z) {
			segments.erase(segments.begin());
		}

		// Adding new Segments
		if (glm::distance(
			segments[segments.size() - 1].position - glm::vec4(0.0, 0.0, segments[segments.size() - 1].length, 0.0),
			status.cameraPosition
		) < renderDistance) {

			int index = std::rand() % baseSegments.size();

			Segment newSegment = baseSegments[index];

			newSegment.position = segments[segments.size() - 1].position - glm::vec4(0.0, 0.0, segments[segments.size() - 1].length, 0.0);

			segments.push_back(newSegment);
		}

		// Adding new CorridorSegments
		if (glm::distance(
			status.cameraPosition,
			corridorSegments[corridorSegments.size() - 1].position - glm::vec4(0.0, 0.0, corridorSegments[corridorSegments.size() - 1].length, 0.0)
		) < renderDistance) {
			CorridorSegment corridorSegment = corridorBase;

			corridorSegment.position = corridorSegments[corridorSegments.size() - 1].position - glm::vec4(0.0, 0.0, corridorSegments[corridorSegments.size() - 1].length, 0.0);

			corridorSegments.push_back(corridorSegment);
		}

		// Removing CorridorSegments
		for (int i = 0; i < corridorSegments.size(); i++) {
			if (corridorSegments[i].position.z - corridorSegments[i].length > status.cameraPosition.z) {
				corridorSegments.erase(corridorSegments.begin() + i);
			}
		}

		lastFrame = now;

		// Ending game
		if (status.gameOver) {
			status.score += status.time * 50;
			std::cout << "==== Game Over! ====" << std::endl;
			std::cout << "= Score: " << status.score << std::endl;
			std::cout << "====================" << std::endl;
			std::cout << "Press 'r' to restart" << std::endl;
		}
	}
}

// Renders corridor, obstacles and the ball
void Game::Render() {

	// Sending render distance
	glUniform1f(RENDER_DISTANCE_LOCATION, renderDistance);

	// Creating the view matrix
	viewMatrix = glm::lookAt(
		glm::vec3(status.cameraPosition),
		glm::vec3(status.cameraPosition) + glm::vec3(0.0, 0.0, -1.0),
		glm::vec3(0.0, 1.0, 0.0)
	);

	// Sending view matrix
	glUniformMatrix4fv(VIEW_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Recalculating light position to eye coordinates
	glm::vec4 lightPos = viewMatrix * (status.cameraPosition + glm::vec4(0.0, 0.0, -1.0, 0.0));

	glUniform4fv(LIGHT_POSITION_LOCATION, 1, glm::value_ptr(lightPos));

	// Rendering Ball
	ball.Render(GL_TRIANGLES, viewMatrix);

	// Rendering all objects
	for (int i = 0; i < segments.size(); i++) {
		segments[i].Render(viewMatrix, status);
	}

	// Rendering corridor
	for (int i = 0; i < corridorSegments.size(); i++) {
		corridorSegments[i].Render(viewMatrix);
	}

	// Rendering gameover text
	if (status.gameOver) {
		gameoverText.position = status.cameraPosition;
		gameoverText.position.z -= 1.2f;
		gameoverText.Render(GL_TRIANGLES, viewMatrix);
	}

}

// Renders pickups
void Game::RenderPickups() {
	// Sending render distance
	glUniform1f(RENDER_DISTANCE_LOCATION, renderDistance);

	// Sending view matrix
	glUniformMatrix4fv(VIEW_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Recalculating light position to eye coordinates
	glm::vec4 lightPos = viewMatrix * (status.cameraPosition + glm::vec4(0.0, 0.0, -1.0, 0.0));

	glUniform4fv(LIGHT_POSITION_LOCATION, 1, glm::value_ptr(lightPos));

	// Rendering all objects
	for (int i = 0; i < segments.size(); i++) {
		segments[i].RenderPickups(viewMatrix, status);
	}
}