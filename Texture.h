#pragma once
#include <GL\glew.h>

class Texture
{
public:
	Texture();
	~Texture();

	GLuint diffuse;
	GLuint specular;
	GLuint normal;
	GLuint ambient;
	GLuint displacement;
};

