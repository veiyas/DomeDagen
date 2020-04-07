#include "game.hpp"
#include "player.hpp"
#include <sgct/engine.h>


//Define instance
Game* Game::mInstance = nullptr;
unsigned int Game::mUniqueId = 0;

Game::Game()
	: mMvp{ glm::mat4{1.f} }, mLastFrameTime{ -1 }
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

	for (const std::pair<const std::string, Model>& p : mModels)
	{
		output += "\n       " + p.first;
	}
	sgct::Log::Info("%s", output.c_str());
}

void Game::printLoadedAssets() const
{
	mInstance->printModelNames();
	mInstance->printShaderPrograms();
}

void Game::render() const
{	
	//for (const std::unique_ptr<Renderable>& obj : mRenderObjects)
	//{		
	//	obj->render();
	//}

	for (auto& [id, obj] : mInteractObjects)
	{
		obj->render();
	}
}
void Game::addObject(std::shared_ptr<Renderable> obj)
{
	//TODO implement shared ptr functionality between mInteractObjects and mRenderObjects
}
void Game::addGameObject(std::unique_ptr<GameObject> obj)
{
	if (sgct::Engine::instance().isMaster())
		mInteractObjects.push_back(std::make_pair(mUniqueId++, std::move(obj)));
}

void Game::addGameObject(std::tuple<unsigned int, std::string>&& inputTuple)
{
	if (sgct::Engine::instance().isMaster())
	{
		std::unique_ptr<GameObject> tempPlayer{
		new Player("fish", 50.f, glm::quat(glm::vec3(0.f,0.f,0.f)), 0.f, std::get<1>(inputTuple) , 0.5f) };
		
		mInteractObjects.push_back(std::make_pair(std::get<0>(inputTuple), std::move(tempPlayer)));
		mUniqueId++;
	}
}

void Game::addGameObject(std::unique_ptr<GameObject> obj, unsigned id)
{
	if (sgct::Engine::instance().isMaster())
	{
		mInteractObjects.push_back(std::make_pair(id, std::move(obj)));
		mUniqueId++;
	}
}

void Game::addRenderable(std::unique_ptr<Renderable> obj)
{
	if (sgct::Engine::instance().isMaster())
		mRenderObjects.push_back(std::move(obj));
}

void Game::update()
{
	if (mLastFrameTime == -1) //First update?
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

std::vector<std::byte> Game::getEncodedPositionData() const
{
	if (sgct::Engine::instance().isMaster())
	{
		std::vector<PositionData> allPositionData(mInteractObjects.size());

		//HOPEFULLY THIS ACCESS THE CORRECT OBJECT
		for (size_t i = 0; i < mInteractObjects.size(); i++)
		{
			allPositionData[i] = mInteractObjects[i].second->getMovementData(mInteractObjects[i].first);
		}

		std::vector<std::byte> tempEncodedData;

		sgct::serializeObject(tempEncodedData, allPositionData);
		allPositionData.clear();

		return tempEncodedData;
	}
}

void Game::setDecodedPositionData(const std::vector<PositionData>& newState)
{
	//TODO This function seems kind of error prone
	if (!sgct::Engine::instance().isMaster())
	{
		for (const auto& newData : newState)
		{			
			if (newState.size() < mInteractObjects.size())
				sgct::Log::Warning("newState.size() < mInteractObjects.size()");

			//If the object doesn't exist on this node yet
			if (newState.size() > mInteractObjects.size())
			{

				//UGLY SOLUTION, create temp objects to update afterwards
				//TODO fix this ugly shit
				while (mInteractObjects.size() != newState.size())
				{
					auto tempPlayer = std::unique_ptr<GameObject>{ new Player("fish", 10.f, glm::quat(glm::vec3(2.f, 0.f, 0.f)), 0.f, "hejhej", 0.f) };
					mInteractObjects.push_back(std::make_pair(mUniqueId++, std::move(tempPlayer)));
				}
				std::cout << "obj size: " << mInteractObjects.size() << ", newState size: " << newState.size() << "\n";
			}
			mInteractObjects[newData.mId].second->setMovementData(newData);
		}
	}
}

void Game::updateTurnSpeed(std::tuple<unsigned int, float>&& input)
{
	unsigned id = std::get<0>(input);
	float rotAngle = std::get<1>(input);

	//Unsure if this is a good way of finding GameObject
	//Looks kinda ugly and could probably be put in seperate method
	//TODO implemented faster search function (mInteractObjects should be sorted)
	auto it = std::find_if(mInteractObjects.begin(), mInteractObjects.end(),
		[id](std::pair<unsigned int, std::unique_ptr<GameObject>>& pair)
			{ return pair.first == id; });

	//If object is not found something has gone wrong
	assert(it != mInteractObjects.end());

	(*it).second->setTurnSpeed(rotAngle);
}

void Game::rotateAllGameObjects(float newOrientation)
{
	for (auto& [id, obj] : mInteractObjects)
	{
		obj->setOrientation(obj->getOrientation() + newOrientation);
	}
}

const Model& Game::getModel(const std::string& nameKey)
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
	std::string vert, frag;

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
		sgct::Log::Error("ERROR OPENING SHADER FILE: %s", shaderName.c_str());
	}
	in_vert.close(); in_frag.close();

	mShaderNames.push_back(shaderName);
	sgct::ShaderManager::instance().addShaderProgram(shaderName, vert, frag);
}
