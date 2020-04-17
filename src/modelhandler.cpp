#include "modelhandler.hpp"

ModelManager* ModelManager::mInstance = nullptr;

void ModelManager::init()
{
	mInstance = new ModelManager();
}

ModelManager& ModelManager::instance()
{
	if (!mInstance)
		mInstance = new ModelManager();
	return *mInstance;
}

const Model& ModelManager::getModel(const std::string& nameKey)
{
	return mModels[nameKey];
}

ModelManager::ModelManager()
{
	for (const auto& modelName : allModelNames)
		loadModel(modelName);
	printModelNames();
}

void ModelManager::loadModel(const std::string& modelName)
{
	std::string path = Utility::findRootDir() + "/src/models/" + modelName + "/" + modelName + ".fbx";
	mModels[modelName] = Model(path.data());
}

void ModelManager::printModelNames() const
{
	std::string output = "Loaded models:";

	for (const std::pair<const std::string, Model>& p : mModels)
	{
		output += "\n       " + p.first;
	}
	sgct::Log::Info("%s", output.c_str());
}
