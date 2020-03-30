#include "game.hpp"


//Define instance
Game* Game::mInstance = nullptr;
std::map<std::string, Model> Game::mModels;

Game::Game()
	: mMvp{ glm::mat4{1.f} }
{
	//Loads all models and shaders into pool
	for (const std::string& modelName : allModelNames)
		loadModel(modelName);

	for (const std::string& shaderName : allShaderNames)
		loadShader(shaderName);
}

void Game::init()
{
	mInstance = new Game{};
	mInstance->printLoadedAssets();
}

Game& Game::getInstance()
{
	//If Game doesnt exist, create one. Return it.
	if (!mInstance) {
		mInstance = new Game{};
	}
	return *mInstance;
}

void Game::destroy()
{
	delete mInstance;
}

void Game::printShaderPrograms() const
{
	std::string output = "Loaded shaders:";
	for (const std::string& s : mShaderNames)
	{
		output +="\n       " + s;
	}
	sgct::Log::Info(output.c_str());
}

void Game::printModelNames() const
{
	std::string output = "Loaded models:";
	for (const std::pair<std::string, Model>& p : mModels)
	{
		output += "\n       " + p.first;
	}
	sgct::Log::Info(output.c_str());
}

void Game::printLoadedAssets() const
{
	mInstance->printModelNames();
	mInstance->printShaderPrograms();
}

void Game::render() const
{
	//TODO This method needs some thoughts regarding renderable vs gameobject rendering
	for (const Renderable* obj : mRenderObjects)
	{		
		obj->render();
	}
	for (const GameObject* obj : mInteractObjects)
	{
		obj->render();
	}
}
void Game::addGameObject(GameObject* obj)
{
	mInteractObjects.push_back(obj);
}

Model& Game::getModel(const std::string& nameKey)
{
	return mModels[nameKey];
}

void Game::loadModel(const std::string& modelName)
{
	std::string path = Utility::findRootDir() + "/src/models/" + modelName + "/" + modelName + ".fbx";
	mModels.emplace(modelName, Model{ path.data() });
}

void Game::loadShader(const std::string& shaderName)
{
	//Define path and strings to hold shaders
	std::string path = Utility::findRootDir() + "/src/shaders/" + shaderName;
	std::string vert = "";
	std::string frag = "";

	//Open streams to shader files
	std::ifstream in_vert{ path + "vert.glsl" };
	std::ifstream in_frag{ path + "frag.glsl" };
	
	//Read shaders into strings
	if (in_vert.good() && in_frag.good()) {
		vert = std::string(std::istreambuf_iterator<char>(in_vert), {});
		frag = std::string(std::istreambuf_iterator<char>(in_frag), {});
	}
	else
	{
		std::cout << "ERROR OPENING SHADER FILE: " + shaderName;
	}
	in_vert.close(); in_frag.close();

	mShaderNames.push_back(shaderName);
	sgct::ShaderManager::instance().addShaderProgram(shaderName, vert, frag);
}
