#include "game.hpp"
#include "player.hpp"
#include <sgct/engine.h>


//Define instance
Game* Game::mInstance = nullptr;
unsigned int Game::mUniqueId = 0;

Game::Game()
	: mMvp{ glm::mat4{1.f} }, mLastFrameTime{ -1 }, mLastSyncedPlayer{0}
{
	//Loads all models and shaders into pool
	for (const std::string& shaderName : allShaderNames)
		loadShader(shaderName);
}

void Game::detectCollisions()
{
	//TODO Rework collision detection when collectibles are implemented
	//if (mInteractObjects.size() > 1)
	//{
	//	for (size_t i = 0; i < mInteractObjects.size(); i++)
	//	{
	//		for (size_t j = i + 1; j < mInteractObjects.size(); j++)
	//		{
	//			auto quat1 = (mInteractObjects[i].second->getPosition());
	//			auto quat2 = (mInteractObjects[j].second->getPosition());

	//			auto z = glm::normalize(glm::inverse(quat1) * quat2);

	//			//Collision detection by comparing how small the angle between the fishes are
	//			//From https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	//			auto sinxPart = 2.f * (z.w * z.x + z.y * z.z);
	//			auto cosxPart = 1.f - 2.f * (z.x*z.x + z.y*z.y);
	//			auto xAngle = std::atan2(sinxPart, cosxPart);
	//			
	//			auto sinyPart = 2.f * (z.w * z.y - z.z * z.x);
	//			auto yAngle = std::asin(sinyPart);

	//			if (std::abs(xAngle) <= collisionDistance && std::abs(yAngle) <= collisionDistance)
	//			{
	//				std::cout << i << " <=> " << j << " collided\n";
	//			}
	//		}
	//	}
	//}
}

void Game::init()
{
	mInstance = new Game{};
	mInstance->mPlayers.reserve(mMAXPLAYERS);
	mInstance->printLoadedAssets();
	BackgroundObject* background = new BackgroundObject("fish", 100, 100, glm::quat(glm::vec3(0.f)));
	mInstance->setBackground(*background);
}

Game& Game::instance()
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

void Game::printLoadedAssets() const
{
	mInstance->printShaderPrograms();
}

void Game::render() const
{
	//Render background
	mBackground->drawBackground(mMvp);

	//Render players
	for (const Player& p : mPlayers)
		p.render(mMvp);

	//TODO render rest of objects
}

void Game::addPlayer()
{
	mPlayers.emplace_back();
}

void Game::addPlayer(const PlayerData& p)
{
	//Create player from PositionData object
	mPlayers.emplace_back(p);
}

void Game::addPlayer(std::tuple<unsigned int, std::string>&& inputTuple)
{
	assert(std::get<0>(inputTuple) == mPlayers.size() && "Player creation desync (id out of bounds: mPlayers)");
	mPlayers.emplace_back(std::get<1>(inputTuple));
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

	//Update players
	for (auto& player : mPlayers)
		player.update(deltaTime);

	detectCollisions();

	//TODO Update other type of objects

	mLastFrameTime = currentFrameTime;
}

std::vector<std::byte> Game::getEncodedPlayerData()
{
	std::vector<PlayerData> allPositionData(mPlayers.size());

	//Sync data for players present on all nodes first
	for (size_t i = 0; i < mLastSyncedPlayer; ++i)
	{
		allPositionData[i] = mPlayers[i].getPlayerData(false);
	}
	for (size_t i = mLastSyncedPlayer; i < mPlayers.size(); ++i)
	{
		allPositionData[i] = mPlayers[i].getPlayerData(true);
	}

	std::vector<std::byte> tempEncodedData;

	sgct::serializeObject(tempEncodedData, allPositionData);
	allPositionData.clear();

	mLastSyncedPlayer = mPlayers.size();

	return tempEncodedData;
}

void Game::setDecodedPositionData(const std::vector<PlayerData>& newState)
{
	if (!sgct::Engine::instance().isMaster())
	{
		//number of unsynced players
		size_t nUnsyncedPlayers = mPlayers.size();

		//New players get instanciated with correct value from the start
		if (newState.size() > mPlayers.size())
		{
			for (size_t i = nUnsyncedPlayers; i < newState.size(); ++i)
			{
				addPlayer(newState[i]);
			}
		}

		for (size_t i = 0; i < nUnsyncedPlayers; ++i)
		{
			mPlayers[i].setPlayerData(newState[i]);
		}
	}
}

void Game::updateTurnSpeed(std::tuple<unsigned int, float>&& input)
{
	unsigned id = std::get<0>(input);
	float rotAngle = std::get<1>(input);

	assert(id < mPlayers.size() && "Player update turn speed desync (id out of bounds mPlayers");

	mPlayers[id].setTurnSpeed(rotAngle);
}

void Game::enablePlayer(unsigned id)
{
	assert(id < mPlayers.size() && "Player disable desync (id out of bounds mPlayers");
	mPlayers[id].enablePlayer();
}

void Game::disablePlayer(unsigned id)
{
	assert(id < mPlayers.size() && "Player disable desync (id out of bounds mPlayers");
	mPlayers[id].disablePlayer();
}

void Game::rotateAllPlayers(float newOrientation)
{
	for (auto& player : mPlayers)
	{
		player.setOrientation(player.getOrientation() + newOrientation);
	}
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
