#pragma once
#include <glm/glm.hpp>
#include "glsl.h"
#include <vector>
#include "Material.cpp"
using namespace std;

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

static float f_mx = std::numeric_limits<float>::max();

static Model empty_model = {
	vector<glm::vec3>{ glm::vec3(f_mx, f_mx, f_mx) },
	vector<glm::vec3>{ glm::vec3(f_mx, f_mx, f_mx) },
	vector<glm::vec2>{ glm::vec2(f_mx, f_mx) },
	{
		glm::vec3(f_mx, f_mx, f_mx),
		glm::vec3(f_mx, f_mx, f_mx),
		glm::vec3(f_mx, f_mx, f_mx),
		f_mx,
		false
	},
	glm::mat4(),
	glm::mat4(),
	GLuint{ UINT_MAX },
	GLuint{ UINT_MAX },
};