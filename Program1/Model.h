
#pragma once
#include <glm/glm.hpp>
#include "glsl.h"
#include <vector>
#include "Material.cpp"
using namespace std;

class Model {
public:
	// Mesh variables
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;

	Material material;
	glm::mat4 model;
	glm::mat4 mv;
	GLuint vao;
	GLuint textureID;

	Model(const char* objectPath, const char* texturePath, Material material, glm::mat4 modelMatrix);
};