#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string.h> 
#include <vector>
#include <glm/glm.hpp>
#include "objloader.hpp"
#include "texture.hpp"
#include "Model.cpp"

using namespace std;

class ModelHandler
{
public:
	ModelHandler() = default;

	void addModel(std::string name, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs);
	void addModel(std::string name, Model model);
	Model* getModel(string name);
	int getModelCount();
	std::map<string, Model>::iterator getModelsIterator();
	std::map<string, Model>::iterator getLastModelIterator();
	void initModel(string name);
	void loadObject(string name, string objPath, string texturePath);
	void loadMaterialsLight(string name, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specular, int power, bool applied); 
	void printAll();
	bool checkModelComplete(string name);

private:
	bool checkModel(string name);
	std::map<string, Model> models;
};

struct ModelNotFoundException : public exception {
	const char * what() const throw () {
		return "ModelNotFoundException";
	}
};
