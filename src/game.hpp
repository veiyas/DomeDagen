#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <tuple>
#include <cmath>

#include "sgct/mutexes.h"
#include "sgct/shareddata.h"
#include "sgct/log.h"
#include "glad/glad.h"
#include "glm/packing.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glm/matrix.hpp"

#include "player.hpp"
#include "utility.hpp"

// abock;  consider implementing all of this as an "implicit" singleton.  Instead of
// making the functions static, you create a single instance of Game in the main.cpp and
// then pass this around.  Personally, I enjoy that method of handling singletons better
// see also https://github.com/OpenSpace/OpenSpace/blob/master/include/openspace/engine/globals.h
// and https://github.com/OpenSpace/OpenSpace/blob/master/src/engine/globals.cpp for a
// way to implement that functionality

//Implemented as explicit singleton, handles pretty much everything
class Game
{
public:
	//Init instance and print useful shader and model info
	static void init();

	//Get instance
	static Game& instance();

	//Check if instance is running
	static bool instanceExists() { return mInstance != nullptr; }

	//Destroy instance
	static void destroy();

	//Copying forbidden
	Game(Game const&) = delete;
	void operator=(Game const&) = delete;

	//Print loaded assets (shaders, models)
	void printLoadedAssets() const;    

	//Render objects
	void render() const;

	//Set MVP matrix
	void setMVP(const glm::mat4& mvp) { mMvp = mvp;};

	//Mostly used for debugging
	void addPlayer();

	//Add player from playerdata for instant sync
	void addPlayer(const PlayerData& p);

	//Add player from server request
	void addPlayer(std::tuple<unsigned int, std::string>&& inputTuple);

	//Update all gameobjects
	void update();

	//ALEX'S WISDOM: LET'S NOT GIVE ACCESS TO THIS CLASS INTERNALS OUTSIDE
	//WRAP EVERYTHING IN METHODS THAT WONT EXPOSE INTERNALS

	//Get and encode position data
	std::vector<std::byte> getEncodedPlayerData();

	//Set position data from inputted data
	void setDecodedPositionData(const std::vector<PlayerData>& newState);

	//Set the turn speed of player player with id id
	void updateTurnSpeed(std:: tuple<unsigned int, float>&& input);
    
    //enable/disable player 
    void enablePlayer(unsigned id);
    void disablePlayer(unsigned id);

	//DEBUGGING TOOL: apply orientation to all GameObjects
	void rotateAllPlayers(float deltaOrientation);

private:
//Members
	//Singleton instance of game
	static Game* mInstance;

	//All players stored sequentually
	std::vector<Player> mPlayers;

	//TODO add collectible storage

	//GameObjects unique id generator for player tagging
	static unsigned int mUniqueId;

	//Track all loaded shaders' names
	std::vector<std::string> mShaderNames;

	//MVP matrix used for rendering
	glm::mat4 mMvp;

	//Slot after which players only present on master node exist
	size_t mLastSyncedPlayer;

	//The time of the last update (in seconds)
	float mLastFrameTime;

	static constexpr double collisionDistance = 0.2f; //TODO make this object specific
	static constexpr size_t mMAXPLAYERS = 110;
	static constexpr size_t mMAXCOLLECTIBLES = 300;

//Functions
	//Constructor
	Game();

	//Collision detection in mInteractObjects, bubble style
	void detectCollisions();

	//Read shader into ShaderManager
	void loadShader(const std::string& shaderName);

	//Display current list of shaders, called by printLoadedAssets()
	void printShaderPrograms() const;

	const glm::mat4& getMVP() { return mMvp; };
};
