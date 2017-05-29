#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <iostream>
#include <vector>

#include "Common.h"

#pragma once
class Model
{
public:
	Model();
	~Model();

	GLuint vao;
	GLuint vbos[NUM_VBOS];

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> tangents;
	std::vector<float> binormals;
	std::vector<int> indices;
	std::vector<float> texCoords;

	glm::vec3 position;
	glm::vec3 localRotation;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec3 localScale;
	glm::vec3 centerMass;
	int restartIndex;

	bool LoadModelFromFile(std::string fileName);
	void Buffer();
	void Render(GLenum drawMode, glm::mat4 viewMatrix);
	glm::vec3 GetCenterMass();
	glm::mat4 GetTransform();
	bool HasTexCoords();

	void Dispose();
};

