#pragma once
#include "glm/glm.hpp"
#include "Model.h"
#include "GameStatus.h"

class Pickup
{
public:
	glm::vec4 position;
	float score;
	Model model;
	int lane;
	bool isActive;
	float collisionDistance = 1.0f;

	Pickup();
	~Pickup();

	GameStatus CheckCollision(GameStatus status);
	void Render(glm::mat4 viewMatrix);
};

