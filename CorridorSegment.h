#pragma once
#include "glm/glm.hpp"
#include "Model.h"
#include <stdio.h>

class CorridorSegment
{
public:
	glm::vec4 position = glm::vec4(0.0, 0.0, 0.0, 1.0);
	float length = 4.0f;

	std::vector<Model> models;

	CorridorSegment();
	~CorridorSegment();

	void Render(glm::mat4 viewMatrix);
};

