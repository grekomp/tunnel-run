#pragma once
#include "glm/glm.hpp"

class GameStatus
{
public:
	GameStatus();
	~GameStatus();

	glm::vec4 cameraPosition;
	float score;
	float time;
	int lane;
};

