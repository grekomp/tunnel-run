#include "Model.h"

Model::Model()
{
	restartIndex = -1;
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	localScale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Model::~Model()
{
}

// ----------------
// Loads a model from file using assimp
bool Model::LoadModelFromFile(std::string fileName) {

	// ----------------
	// Importing from obj file
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fileName, aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene) {
		std::cout << importer.GetErrorString();
	}
	else {

		restartIndex = scene->mMeshes[0]->mNumVertices;

		for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
			// Add all vertices to a Model instance
			vertices.push_back(scene->mMeshes[0]->mVertices[i].x);
			vertices.push_back(scene->mMeshes[0]->mVertices[i].y);
			vertices.push_back(scene->mMeshes[0]->mVertices[i].z);
			vertices.push_back(1.0f);

			// Add all normals
			normals.push_back(scene->mMeshes[0]->mNormals[i].x);
			normals.push_back(scene->mMeshes[0]->mNormals[i].y);
			normals.push_back(scene->mMeshes[0]->mNormals[i].z);

			// Add all tangents
			tangents.push_back(scene->mMeshes[0]->mTangents[i].x);
			tangents.push_back(scene->mMeshes[0]->mTangents[i].y);
			tangents.push_back(scene->mMeshes[0]->mTangents[i].z);
			
			// Add all binormals (bitangents)
			binormals.push_back(scene->mMeshes[0]->mBitangents[i].x);
			binormals.push_back(scene->mMeshes[0]->mBitangents[i].y);
			binormals.push_back(scene->mMeshes[0]->mBitangents[i].z);

		}

		// Add texture coordinates if existing
		if (scene->mMeshes[0]->HasTextureCoords(0)) {
			for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
				texCoords.push_back(scene->mMeshes[0]->mTextureCoords[0][i].x);
				texCoords.push_back(scene->mMeshes[0]->mTextureCoords[0][i].y);
			}
		}

		// Add all indices
		for (int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
			for (int j = 0; j < scene->mMeshes[0]->mFaces[i].mNumIndices; j++) {
				indices.push_back(scene->mMeshes[0]->mFaces[i].mIndices[j]);
			}
			indices.push_back(restartIndex);
		}
		
	}


	return true;
}

// ----------------
// Buffers vertices, normals and indices to the gpu
void Model::Buffer() {
	// Creating vao and vbos
	glGenVertexArrays(1, &vao);
	glGenBuffers(NUM_VBOS, vbos);

	// Binding vao
	glBindVertexArray(vao);

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbos[VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), reinterpret_cast<GLfloat *>(&vertices[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES_LOCATION);
	glVertexAttribPointer(VERTICES_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMALS]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), reinterpret_cast<GLfloat *>(&normals[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMALS_LOCATION);
	glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), reinterpret_cast<GLfloat *>(&indices[0]), GL_STATIC_DRAW);

	// Texture Coordinates
	if (HasTexCoords()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos[TEX_COORDS]);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), reinterpret_cast<GLfloat *>(&texCoords[0]), GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEXTURE_COORDINATES_LOCATION);
		glVertexAttribPointer(TEXTURE_COORDINATES_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Tangents
	glBindBuffer(GL_ARRAY_BUFFER, vbos[TANGENTS]);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(float), reinterpret_cast<GLfloat *>(&tangents[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(TANGENTS_LOCATION);
	glVertexAttribPointer(TANGENTS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Binormals
	glBindBuffer(GL_ARRAY_BUFFER, vbos[BINORMALS]);
	glBufferData(GL_ARRAY_BUFFER, binormals.size() * sizeof(float), reinterpret_cast<GLfloat *>(&binormals[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(BINORMALS_LOCATION);
	glVertexAttribPointer(BINORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Restart index
	if (restartIndex == -1) restartIndex == vertices.size();
	glPrimitiveRestartIndex(restartIndex);

	// Calculating center mass
	centerMass = GetCenterMass();

	// Unbinding vao
	glBindVertexArray(0);
}

// ----------------
// Renders the model
void Model::Render(GLenum drawMode, glm::mat4 viewMatrix) {

	// Binding material
	material.Bind();

	// Binding vao
	glBindVertexArray(vao);

	// Setting primitive restart index
	glPrimitiveRestartIndex(restartIndex);

	// Applying transformations
	glm::mat4 modelMatrix = GetTransform();
	glUniformMatrix4fv(MODEL_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Normal transformation matrix
	glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(viewMatrix * modelMatrix));
	glUniformMatrix3fv(NORMAL_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(normalMat));

	// Drawing elements
	glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
}

// ----------------
// Calculates and returns the center of mass
glm::vec3 Model::GetCenterMass() {
		glm::vec3 center = { 0.0, 0.0, 0.0 };

		for (int i = 0; i < vertices.size(); i += 4) {
			center.x += vertices[i];
			center.y += vertices[i + 1];
			center.z += vertices[i + 2];
		}

		center.x /= (float)(vertices.size() / 4);
		center.y /= (float)(vertices.size() / 4);
		center.z /= (float)(vertices.size() / 4);

		return center;
}

// ----------------
// Returns a model transform matrix
glm::mat4 Model::GetTransform() {
	// Global rotation
	glm::mat4 transform = glm::rotate(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform *= glm::rotate(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform *= glm::rotate(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	// Translation
	transform *= glm::translate(position);
	
	// Global scale
	transform *= glm::scale(scale);

	// Return to original position
	transform *= glm::translate(centerMass);

	// Local rotation
	transform *= glm::rotate(glm::radians(localRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform *= glm::rotate(glm::radians(localRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform *= glm::rotate(glm::radians(localRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	// Local scale
	transform *= glm::scale(localScale);

	// Move center mass to (0, 0, 0)
	transform *= glm::translate(-centerMass);

	return transform;
}

// ----------------
// Specifies whether the model has texture coordinates
bool Model::HasTexCoords() {
	return texCoords.size() > 0;
}

// ----------------
// Deletes the vbos and vao
void Model::Dispose() {
	glDeleteBuffers(NUM_VBOS, vbos);
	glDeleteVertexArrays(1, &vao);
}
