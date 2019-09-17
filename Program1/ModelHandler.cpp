#include "ModelHandler.h"
#include <glm/gtc/type_ptr.hpp>

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
	models[name] = empty_model;
}

void ModelHandler::loadObject(string name, string objPath, string texturePath) {
	if (checkModelExists(name)) {
		cout << "Object [" << name << "] loaded." << endl;
		Model* model = &models[name];
		bool res = loadOBJ(objPath.c_str(), model->vertices, model->uvs, model->normals);
		model->textureID = loadBMP(texturePath.c_str());
	}
}

void ModelHandler::loadMaterialsLight(string name, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specular, int power, bool applied) {
	if (checkModelExists(name)) {
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

bool ModelHandler::checkModelExists(string name) {
	if (models.find(name) != models.end())
		return true;
	throw new ModelNotFoundException();
}

void ModelHandler::printModel(Model modelToPrint, string name)
{
	cout << endl << "/---- Model: " << name << " ----\\" << endl;
	cout << "vertices: " << modelToPrint.vertices.size() << endl;
	cout << "normals: " << modelToPrint.normals.size() << endl;
	cout << "uvs: " << modelToPrint.uvs.size() << endl;
	cout << "material: " << endl;
	cout << "\t ambientColor: (" << modelToPrint.material.ambientColor.x << ", " << modelToPrint.material.ambientColor.y << ", " << modelToPrint.material.ambientColor.z << ")" << endl;
	cout << "\t diffuseColor: (" << modelToPrint.material.diffuseColor.x << ", " << modelToPrint.material.diffuseColor.y << ", " << modelToPrint.material.diffuseColor.z << ")" << endl;
	cout << "\t specular: (" << modelToPrint.material.specular.x << ", " << modelToPrint.material.specular.y << ", " << modelToPrint.material.specular.z << ")" << endl;
	cout << "\t power: " << modelToPrint.material.power << endl;
	cout << "\t applied: " << modelToPrint.material.applied << endl;
	cout << "model: " << endl;
	const float *pSource = (const float*)glm::value_ptr(modelToPrint.model);
	for (int i = 0; i < 16; i++)
	{
		if (i % 4 == 0)
		{
			cout << "\t";
		}
		cout << pSource[i];
		if (i % 4 == 3)
		{
			cout << endl;
		}
		else
		{
			cout << ", ";
		}

	}
	cout << "mv: " << endl;
	pSource = (const float*)glm::value_ptr(modelToPrint.mv);
	for (int i = 0; i < 16; i++)
	{
		if (i % 4 == 0) { cout << "\t"; }
		cout << pSource[i];
		if (i % 4 == 3)
		{
			cout << endl;
		}
		else
		{
			cout << ", ";
		}

	}
	cout << "vao: " << modelToPrint.vao << endl;
	cout << "textureID: " << modelToPrint.textureID << endl;
	cout << "\\-----------------------/" << endl << endl;
}

void ModelHandler::printModel(string name)
{
	Model model = models.find(name)->second;
	printModel(model, name);
}

bool ModelHandler::checkModelComplete(string name)
{
	if(checkModelExists(name))
	{
		Model model = models[name];
		Model test = empty_model;

		if (model.vertices == empty_model.vertices || 
			model.normals  == empty_model.normals  || 
			model.uvs      == empty_model.uvs      ||
			(
				model.material.ambientColor == empty_model.material.ambientColor &&
				model.material.diffuseColor == empty_model.material.diffuseColor &&
				model.material.specular     == empty_model.material.specular &&
				model.material.power        == empty_model.material.power &&
				model.material.applied      == empty_model.material.applied
			) || model.textureID == empty_model.textureID)
			// model.model == empty_model.model  ||
			// model.mv    == empty_model.mv     ||  These are not included because they will be changed by OpenGL while rendering.
			// model.vao   == empty_model.vao    ||
		{
			cout << name << " model is incomplete." << endl;
			return false;
		}
	}else
	{
		cout << name << " model doesn't exist." << endl;
	}
	cout << "Model " << name << " is complete" << endl;
	return true;
}
