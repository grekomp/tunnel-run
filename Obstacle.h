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
	float collisionDistance = 0.3f;
	bool isActive = true;

	Obstacle();
	~Obstacle();

	void Render(glm::mat4 viewMatrix, glm::vec4 segmentPosition, GameStatus status);
	bool CheckCollision(GameStatus status, glm::vec4 segmentPosition);
	void Dispose();
};

