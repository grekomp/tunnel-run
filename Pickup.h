#pragma once
#include "glm/glm.hpp"
#include "Model.h"
#include "GameStatus.h"
#include <math.h>

class Pickup
{
public:
	glm::vec4 position;
	float score;
	Model model;
	int lane;
	bool isActive = true;
	float collisionDistance = 1.0f;
	GLuint cubemapTexture;
	float rotationSpeed = 10.0f;

	Pickup();
	~Pickup();

	GameStatus CheckCollision(GameStatus status);
	void Render(glm::mat4 viewMatrix, glm::vec4 segmentPosition, GameStatus status);
	void Dispose();
};

