#pragma once
#include "glm/glm.hpp"
#include "Obstacle.h"
#include "Pickup.h"

class Segment
{
public:
	std::vector<Obstacle> obstacles;
	std::vector<Pickup> pickups;

	float length;
	glm::vec4 position;

	GameStatus CheckCollisions(GameStatus status);
	void Dispose();
	void Render(glm::mat4 viewMatrix);
	void RenderPickups(glm::mat4 viewMatrix, GameStatus status);

	Segment();
	~Segment();
};

