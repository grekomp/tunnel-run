#pragma once
#include "glm/glm.hpp"
#include "Model.h"
#include "GameStatus.h"
#include "Material.h"

class Obstacle
{
public:
	glm::vec4 position;
	Model model;
	int lane;
	float collisionDistance = 1.0f;

	Obstacle();
	~Obstacle();

	void Render(glm::mat4 viewMatrix);
	bool CheckCollision(GameStatus status);
	void Dispose();
};

