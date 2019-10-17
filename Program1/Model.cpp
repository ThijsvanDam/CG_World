#pragma once
#include <glm/glm.hpp>
#include "glsl.h"
#include <utility>
#include <vector>
#include "Material.cpp"
#include "Model.h"
#include "objloader.hpp"
#include "texture.hpp"

using namespace std;

Model::Model(const char* objectPath, const char* texturePath, Material i_material, glm::mat4 modelMatrix) {
	// position, rotation and scale for this object;
	model = modelMatrix;

	// init material
	textureID = loadBMP(texturePath);
	material = i_material;

	// init object
	loadOBJ(objectPath, vertices, uvs, normals);
};

//static float f_mx = std::numeric_limits<float>::max();
//
//static Model empty_model = {
//	vector<glm::vec3>{ glm::vec3(f_mx, f_mx, f_mx) },
//	vector<glm::vec3>{ glm::vec3(f_mx, f_mx, f_mx) },
//	vector<glm::vec2>{ glm::vec2(f_mx, f_mx) },
//	{
//		glm::vec3(f_mx, f_mx, f_mx),
//		glm::vec3(f_mx, f_mx, f_mx),
//		glm::vec3(f_mx, f_mx, f_mx),
//		f_mx,
//		false
//	},
//	glm::mat4(),
//	glm::mat4(),
//	GLuint{ UINT_MAX },
//	GLuint{ UINT_MAX },
//};