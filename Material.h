#pragma once
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Common.h"

class Material
{
public:
	Material();
	~Material();

	Texture texture;
	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec3 specular;
	float shininess = 50.0f;
	float parallaxStrength = 0.1f;
	bool ignoreTextures = false;

	void Bind();
};

