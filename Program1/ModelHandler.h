//#pragma once
//#include <vector>
//#include <map>
//#include "Model.cpp"
//#include "objloader.hpp"
//#include "texture.hpp"
//#include <iostream>
//#include <string>
//
//using namespace std;
//
//
//class ModelHandler
//{
//public:
//	ModelHandler() = default;
//	void addModel(std::string name, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs);
//	void addModel(std::string name, Model model);
//	Model* getModel(string name);
//	int getModelCount();
//	std::map<string, Model>::iterator getModelsIterator();
//	std::map<string, Model>::iterator getLastModelIterator();
//	void initModel(string name);
//	void loadObject(string name, string objPath, string texturePath);
//	void loadMaterialsLight(string name, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specular, int power, bool applied); 
//	void printModel(string name); 
//	void printModel(Model model, string name); 
//	void printAll();
//	bool checkModelComplete(string name);
//
//private:
//	bool checkModelExists(string name);
//	// TODO: Might want to change the models map to be <string, Model*>
//	std::map<string, Model> models;
//};
//
//struct ModelNotFoundException : public exception {
//	const char * what() const throw () {
//		return "ModelNotFoundException";
//	}
//};