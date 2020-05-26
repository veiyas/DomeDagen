#include "modelmanager.hpp"

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

Model& ModelManager::getModel(const std::string& nameKey)
{
	const auto& foundPair = std::find_if(mModels.begin(), mModels.end(),
		[nameKey](std::pair<std::string, Model>& pair)
		{
			return pair.first == nameKey;
		});
	return (*foundPair).second;
}

Model& ModelManager::getModel(const int index)
{
	return mModels[index].second;
}

int ModelManager::findModelSpot(const std::string& nameKey)
{
	for (int i = 0; i < mModels.size(); i++)
	{
		if (mModels[i].first == nameKey)
			return i;
	}
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
	mModels.push_back(std::make_pair(modelName, Model(path.data())));
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
