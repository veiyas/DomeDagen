#pragma once

#include <map>
#include <string>

#include "sgct/log.h"

#include "utility.hpp"
#include "constants.hpp"
#include "model.hpp"

//Explicit singleton class to store models to reduce coupling
class ModelManager
{
public:
	//Initialize instance and loads models
	static void init();

	//Dtor for cleanup
	~ModelManager() { delete mInstance; }

	//Get instance
	static ModelManager& instance();

	//Get model for objects
	Model& getModel(const std::string& nameKey);
	Model& getModel(const int index);

	//Find model spot in mModels
	int findModelSpot(const std::string& nameKey);

private:
	//The singleton instance, ctor that loads models
	static ModelManager* mInstance;
	ModelManager();	

	//All models loaded into a pool (vertices + textures, see mesh.hpp)
	void loadModel(const std::string& modelName);

	//Model
	std::vector<std::pair<std::string, Model>> mModels;

	void printModelNames() const;
};