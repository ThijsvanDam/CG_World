#include "ModelHandler.h"

#include "Model.cpp"

using namespace std;


void ModelHandler::addModel(std::string name, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs) {
	this->addModel(name, { vertices, normals, uvs });
}

void ModelHandler::addModel(string name, Model model) {
	this->models[name] = model;
}

Model* ModelHandler::getModel(string name)
{
	return &this->models[name];
}


int ModelHandler::getModelCount()
{
	return models.size();
}

std::map<string, Model>::iterator ModelHandler::getModelsIterator()
{
	return models.begin();
}

std::map<string, Model>::iterator ModelHandler::getLastModelIterator()
{
	return models.end();
}

void ModelHandler::initModel(string name)
{
	cout << "Object [" << name << "] initialized." << endl;
	models[name] = {};
}

void ModelHandler::loadObject(string name, string objPath, string texturePath) {
	if (checkModel(name)) {
		cout << "Object [" << name << "] loaded." << endl;
		Model* model = &models[name];
		bool res = loadOBJ(objPath.c_str(), model->vertices, model->uvs, model->normals);
		model->textureID = loadBMP(texturePath.c_str());
	}
}

void ModelHandler::loadMaterialsLight(string name, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specular, int power, bool applied) {
	if (checkModel(name)) {
		cout << "Lights for [" << name << "] loaded." << endl;
		Model* model = &models[name];
		model->material.ambientColor = ambientColor;
		model->material.diffuseColor = diffuseColor;
		model->material.specular = specular;
		model->material.power = power;
		model->material.applied = applied;
	}
}

void ModelHandler::printAll() {
	for (auto item : models) {
		cout << item.first;
	}
	cout << endl;
}

bool ModelHandler::checkModel(string name) {
	if (models.find(name) != models.end())
		return true;
	throw new ModelNotFoundException();
}

