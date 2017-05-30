#pragma once
#include "glm/glm.hpp"

class GameStatus
{
public:
	GameStatus();
	~GameStatus();

	glm::vec4 cameraPosition = glm::vec4(0, 0, 10, 1);
	float score;
	float time;
	float deltaTime;
	int lane;
	bool gameOver;
};

