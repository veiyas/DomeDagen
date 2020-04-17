#pragma once

#include <map>
#include <string>

#include "sgct/log.h"

#include "utility.hpp"
#include "constants.hpp"
#include "model.hpp"

//Singleton class to store models to reduce coupling
class ModelManager
{
public:
	//Initialize instance and loads models
	static void init();

	//Get instance
	static ModelManager& instance();

	//Get model for objects
	const Model& getModel(const std::string& nameKey);

private:
	//The instance, ctor that loads models
	static ModelManager* mInstance;
	ModelManager();

	//All models loaded into a pool (vertices + textures, see mesh.hpp)
	void loadModel(const std::string& modelName);

	//Model
	std::map<std::string, Model> mModels;

	void printModelNames() const;
};