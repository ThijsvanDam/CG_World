#pragma once

#include <GL/glew.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "glsl.h"
#include <vector>

struct Model {
	// Mesh variables
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;

	Material material;
	glm::mat4 model;
	glm::mat4 mv;
	GLuint vao;
	GLuint textureID;
};