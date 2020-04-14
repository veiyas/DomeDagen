#include "game.hpp"
#include "player.hpp"
#include "collectiblepool.hpp"

#include <sgct/engine.h>

//Define instance and id counter
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

void Game::detectCollisions()
{
	if (mInteractObjects.size() > 1)
	{
		for (size_t i = 0; i < mInteractObjects.size(); i++)
		{
			for (size_t j = i + 1; j < mInteractObjects.size(); j++)
			{
				auto quat1 = (mInteractObjects[i].second->getPosition());
				auto quat2 = (mInteractObjects[j].second->getPosition());

				auto z = glm::normalize(glm::inverse(quat1) * quat2);

				//Collision detection by comparing how small the angle between the fishes are
				//From https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
				auto sinxPart = 2.f * (z.w * z.x + z.y * z.z);
				auto cosxPart = 1.f - 2.f * (z.x*z.x + z.y*z.y);
				auto xAngle = std::atan2(sinxPart, cosxPart);
				
				auto sinyPart = 2.f * (z.w * z.y - z.z * z.x);
				auto yAngle = std::asin(sinyPart);

				if (std::abs(xAngle) <= collisionDistance && std::abs(yAngle) <= collisionDistance)
				{
					//TODO collision interactions
					//std::cout << i << " <=> " << j << " collided\n";
				}
			}
		}
	}
}

void Game::init()
{
	mInstance = new Game{};
	mInstance->printLoadedAssets();
}

Game& Game::getInstance()
{
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
	for (const std::shared_ptr<Renderable>& obj : mRenderObjects)
	{		
		obj->render(getInstance().getMVP());
	}
}

void Game::addGameObject(std::shared_ptr<GameObject> obj)
{
	addGameObject(std::move(obj), mUniqueId);
}

void Game::addGameObject(std::tuple<unsigned int, std::string>&& inputTuple)
{
	std::shared_ptr<GameObject> tempPlayer{
	new Player("fish", 50.f, glm::quat(glm::vec3(0.f,0.f,0.f)), 0.f, std::get<1>(inputTuple) , 0.5f) };
		
	addGameObject(std::move(tempPlayer), std::get<0>(inputTuple));	
}

void Game::addGameObject(std::shared_ptr<GameObject> obj, unsigned& id)
{
	//Copy the shared_ptr to renderables
	addRenderable(obj);

	mInteractObjects.push_back(std::make_pair(id, std::move(obj)));
	mUniqueId++;
}

void Game::addRenderable(std::shared_ptr<Renderable> obj)
{
	mRenderObjects.push_back(std::move(obj));
}

//DEBUGGING PURPOSES
bool outputted = false;
void Game::update()
{
	if (mLastFrameTime == -1) //First update?
	{
		mLastFrameTime = static_cast<float>(sgct::Engine::getTime());
		return;
	}

	float currentFrameTime = static_cast<float>(sgct::Engine::getTime());
	float deltaTime = currentFrameTime - mLastFrameTime;
	

	std::random_device randomDevice;
	std::mt19937 gen(randomDevice());
	std::uniform_real_distribution<> rng(-0.7f, 0.7f);

	//TODO Spawn collectibles
	//COLLECTIBLE SPAWN DEBUGGING
	if ((int)currentFrameTime % 2 == 0 && !outputted)
	{
		addGameObject(CollectiblePool::instance().enableCollectible(glm::vec3(rng(gen)+2.f, rng(gen), 0.f)));
		outputted = true;
	}

	if ((int)currentFrameTime % 2 == 1 || (int)currentFrameTime % 2 == 2)
		outputted = false;

	detectCollisions();

	for (auto& [id, obj] : mInteractObjects)
	{
		obj->update(deltaTime);
	}

	mLastFrameTime = currentFrameTime;
}

std::vector<std::byte> Game::getEncodedPositionData() const
{
	std::vector<PositionData> allPositionData(mInteractObjects.size());

	for (size_t i = 0; i < mInteractObjects.size(); i++)
	{
		allPositionData[i] = mInteractObjects[i].second->getMovementData(mInteractObjects[i].first);
	}

	std::vector<std::byte> tempEncodedData;

	sgct::serializeObject(tempEncodedData, allPositionData);
	allPositionData.clear();

	return tempEncodedData;
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

			if (newState.size() > mInteractObjects.size())
			{
				//UGLY SOLUTION, create temp objects to update afterwards
				//TODO fix this ugly shit
				while (mInteractObjects.size() != newState.size())
				{
					std::shared_ptr<GameObject> tempPlayer{ new Player("fish", 10.f, glm::quat(glm::vec3(2.f, 0.f, 0.f)), 0.f, "temp", 0.f) };
					addGameObject(std::move(tempPlayer));
				}
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
	//TODO implemented faster search function (mInteractObjects is sorted)
	auto it = std::find_if(mInteractObjects.begin(), mInteractObjects.end(),
		[id](std::pair<unsigned int, std::shared_ptr<GameObject>>& pair)
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
