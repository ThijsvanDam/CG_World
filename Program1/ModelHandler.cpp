#include "ModelHandler.h"

using namespace std;


void ModelHandler::addModel(std::string name, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs) {
	this->addModel(name, { vertices, normals, uvs });
}

void ModelHandler::addModel(string name, Model model) {
	this->models[name] = model;
}

Model* ModelHandler::getModel(string name)
{
	// TODO: I don't think this should be a pointer/reference
	return &this->models[name];
}

int ModelHandler::getModelCount()
{
	return this->models.size();
}

std::map<string, Model>::iterator ModelHandler::getModelsIterator()
{
	return this->models.begin();
}

std::map<string, Model>::iterator ModelHandler::getLastModelIterator()
{
	return this->models.end();
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

bool ModelHandler::checkModelComplete(string name)
{
	if(checkModel(name))
	{
		Model model = models[name];
		Model test = empty_model;
		cout << "pinda" << endl;


		if (model.vertices == empty_model.vertices || 
			model.normals == empty_model.normals || 
			model.uvs == empty_model.uvs ||
			(
				model.material.ambientColor == empty_model.material.ambientColor &&
				model.material.diffuseColor == empty_model.material.diffuseColor &&
				model.material.specular == empty_model.material.specular &&
				model.material.power == empty_model.material.power &&
				model.material.applied == empty_model.material.applied
			) ||
			// model.model == empty_model.model ||
			// model.mv == empty_model.mv ||
			model.vao == empty_model.vao ||
			model.textureID == empty_model.textureID)
		{
			cout << name << " model is incomplete." << endl;
			return false;
		}
	}else
	{
		cout << name << " model doesn't exist." << endl;
	}
	cout << name << " model is complete." << endl;
	return true;
}
