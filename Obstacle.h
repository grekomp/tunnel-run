#pragma once
#include "glm/glm.hpp"
#include "Model.h"
#include "GameStatus.h"

class Obstacle
{
public:
	glm::vec4 position;
	Model model;
	int lane;
	float collisionDistance = 1.0f;
	bool CheckCollision(GameStatus status);
	
	Obstacle();
	~Obstacle();

	void Render(glm::mat4 viewMatrix);
};

