#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <mutex>

#include "sgct/mutexes.h"
#include "sgct/shareddata.h"
#include "sgct/log.h"
#include "glad/glad.h"
#include "glm/packing.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glm/matrix.hpp"

#include "renderable.hpp"
#include "gameobject.hpp"
#include "utility.hpp"

//Temp name holders for easier file reading
const std::vector<std::string> allModelNames{ "fish" };
const std::vector<std::string> allShaderNames{ "player", "testing", "sceneobject" };

// abock;  consider implementing all of this as an "implicit" singleton.  Instead of
// making the functions static, you create a single instance of Game in the main.cpp and
// then pass this around.  Personally, I enjoy that method of handling singletons better
// see also https://github.com/OpenSpace/OpenSpace/blob/master/include/openspace/engine/globals.h
// and https://github.com/OpenSpace/OpenSpace/blob/master/src/engine/globals.cpp for a
// way to implement that functionality


//Implemented as singleton, handles pretty much everything
class Game
{
public:
	//Init instance and print useful shader and model info
	static void init();

	//Get instance
	static Game& getInstance();

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

	//Add object to mInteractObjects
	void addGameObject(std::unique_ptr<GameObject> obj);

	//Add object to mRenderObjects
	void addRenderable(Renderable* obj);

	//Update all gameobjects
	void update();

	// abock;  Consider the next five functions carefully.  You are exposing the internal
	// state of the game to the outside, so you can no longer reason about the state after
	// these functions have been called because you don't know what the called will do to
	// them;  basically you are making the member variables public by giving this broad
	// access

	//Get ref to GameObject in slot index, binary searched
	//GameObject& getGameObject(const unsigned searchId);

	//Get ref to all GameObjects
	//std::map<const unsigned int, std::unique_ptr<GameObject>>& getGameObjectMap();

	//Get and encode position data
	std::vector<std::byte> getEncodedPositionData();

	//Set position data from inputted data
	void setDecodedPositionData(std::vector<PositionData>& newState);

	//Accessors
	Model& getModel(const std::string& nameKey);
	glm::mat4& getMVP() { return mMvp; };

	//Debugging tools
	bool showFps = true;

private:
//Members
	//Singleton instance of game
	static Game* mInstance;

	//All models loaded into a pool (vertices + textures, see mesh.hpp)
	std::map<std::string, Model> mModels;

	//All renderable objects
	std::vector<std::unique_ptr<Renderable>> mRenderObjects;

	//All interactble objects in a struct for id access/searching
	std::vector<std::pair<unsigned int, std::unique_ptr<GameObject>>> mInteractObjects;

	//GameObjects unique id generator
	static unsigned int mUniqueId;

	//TODO maybe a separate vector for objects with collision only (performance enhancement)

	//Track all loaded shaders' names
	std::vector<std::string> mShaderNames;

	//MVP matrix used for rendering
	glm::mat4 mMvp;

	//The time of the last update (in seconds)
	float mLastFrameTime;

//Functions
	//Constructor
	Game();

	//Load model into pool
	void loadModel(const std::string& modelName);

	//Read shader into ShaderManager
	void loadShader(const std::string& shaderName);

	//Display current list of shaders, called by printLoadedAssets()
	void printShaderPrograms() const;

	//Display current list of models, called by printLoadedAssets()
	void printModelNames() const;
};
