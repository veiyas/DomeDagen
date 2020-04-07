#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <tuple>

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
const std::vector<std::string> allModelNames{ "fish", "can1", "can2", "can3", "can4", "bottle1", "bottle2", "bottle3", "sixpack1", "sixpack2", "sixpack3" };
const std::vector<std::string> allShaderNames{ "player", "testing", "sceneobject" };

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

	//Add object in general
	void addObject(std::shared_ptr<Renderable> obj);

	//Add object to mInteractObjects
	void addGameObject(std::unique_ptr<GameObject> obj);

	//Add object to mInteractObjects from tuple
	void addGameObject(std::tuple<unsigned int, std::string>&& inputTuple);

	//Add object to mInteractObjects with id
	void addGameObject(std::unique_ptr<GameObject> obj, unsigned id);

	//Add object to mRenderObjects
	void addRenderable(std::unique_ptr<Renderable> obj);

	//Update all gameobjects
	void update();

	//ALEX'S WISDOM: LET'S NOT GIVE ACCESS TO THIS CLASS INTERNALS OUTSIDE
	//WRAP EVERYTHING IN METHODS THAT WONT EXPOSE INTERNALS

	//Get and encode position data
	std::vector<std::byte> getEncodedPositionData() const;

	//Set position data from inputted data
	void setDecodedPositionData(const std::vector<PositionData>& newState);

	//Set the turn speed of player player with id id
	void updateTurnSpeed(std:: tuple<unsigned int, float>&& input);

	//DEBUGGING TOOL: apply orientation to all GameObjects
	void rotateAllGameObjects(float newOrientation);

	//Accessors
	const Model& getModel(const std::string& nameKey);
	glm::mat4& getMVP() { return mMvp; };

private:
//Members
	//Singleton instance of game
	static Game* mInstance;

	//All models loaded into a pool (vertices + textures, see mesh.hpp)
	std::map<std::string, Model> mModels;

	//All renderable objects
	std::vector<std::unique_ptr<Renderable>> mRenderObjects;

	//All interactble objects in pair for id access/searching
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
