#include "game.hpp"
#include <sgct/engine.h>


//Define instance
Game* Game::mInstance = nullptr;
unsigned int Game::mUniqueId = 0;

Game::Game()
	: mMvp{ glm::mat4{1.f} }, mLastFrameTime{ -1 }
{
	//Ctor seems to get called twice times even tho it has already been instanciated
	//It works now but it is ugly

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
	auto test = mInstance->instanceExists();
	//If Game doesnt exist, create one. Return it.
	if (!mInstance) {
		mInstance = new Game{};
	}
	return *mInstance;
}

void Game::destroy()
{
	if(mInstance->instanceExists())
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
	//TODO This method might needs some thoughts regarding renderable vs gameobject rendering
	for (const std::unique_ptr<Renderable>& obj : mRenderObjects)
	{		
		obj->render();
	}
	for (auto& [key, value] : mInteractObjects)
	{
		value->render();
	}
}
void Game::addGameObject(GameObject* obj)
{
	mInteractObjects[mUniqueId++] = std::unique_ptr<GameObject>(std::move(obj));
}

void Game::addRenderable(Renderable* obj)
{
	mRenderObjects.push_back(std::unique_ptr<Renderable>(std::move(obj)));
}

void Game::update()
{
	if (mLastFrameTime < 0) //First update?
	{
		mLastFrameTime = static_cast<float>(sgct::Engine::getTime());
		return;
	}

	float currentFrameTime = static_cast<float>(sgct::Engine::getTime());
	float deltaTime = currentFrameTime - mLastFrameTime;

	for (auto& [id, obj] : mInteractObjects)
	{
		obj->update(deltaTime);
	}

	//TODO check for collisions

	mLastFrameTime = currentFrameTime;
}

GameObject& Game::getGameObject(const unsigned index)
{
	return *mInteractObjects.at(index).get();
}

std::map<const unsigned int, std::unique_ptr<GameObject>>& Game::getGameObjectMap()
{
	return mInteractObjects;
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
