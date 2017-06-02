#pragma once
#include "glm/glm.hpp"

class GameStatus
{
public:
	GameStatus();
	~GameStatus();

	glm::vec4 cameraPosition = glm::vec4(0.0, 0.0, 10.0, 1.0);
	glm::vec4 ballPosition = cameraPosition + glm::vec4(0.0, 0.0, -1.0, 1.0);
	float ballRadius;
	float score;
	float time;
	float deltaTime;
	int lane;
	bool gameOver = false;
	float renderStart = 0.0f;
};

