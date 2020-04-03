#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

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

const std::vector<std::string> allModelNames{ "fish" };
const std::vector<std::string> allShaderNames{ "player", "testing", "sceneobject" };

//Implemented as singleton
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
	void addGameObject(GameObject* obj);

	//Add object to mRenderObjects
	void addRenderable(Renderable* obj);

	//Update all gameobjects
	void update();

	//Get ref to GameObject in slot index
	GameObject& getGameObject(const unsigned index);

	//Get ref to vector of position states used for encoding/decoding
	std::vector<PositionData>& getMovementStates() { return mObjectsPositionStates; }

	//Get ref to all GameObjects
	std::map<const unsigned int, std::unique_ptr<GameObject>>& getGameObjectVector();

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

	//All interactble objects, map because we need unique id's for sync
	std::map<const unsigned int, std::unique_ptr<GameObject>> mInteractObjects;

	//GameObjects unique id generator
	static unsigned int mUniqueId;

	//Track object position states between syncs
	std::vector<PositionData> mObjectsPositionStates;

	//TODO maybe a separate vector for objects with collision only (performance enhancement)

	//Track all shader names
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
