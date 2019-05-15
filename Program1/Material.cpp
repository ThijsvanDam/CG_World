#pragma once
#include <glm/glm.hpp>

using namespace std;

struct Material {
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specular;
	float power;
	bool applied;
};