#ifndef GAME_H_
#define GAME_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glm/matrix.hpp"
#include "renderable.hpp"
#include "gameobject.hpp"
#include "sceneobject.hpp"
#include "model.hpp"
#include "player.hpp"

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

    //Add object to mRenderables
    void addGameObject(GameObject* obj);

    //Accessors
    Model& getModel(const std::string& nameKey);
    glm::mat4& getMVP() { return mMvp; };

private:
        //Members
    //Singleton instance of game
    static Game* mInstance;

    //All models loaded into a pool (vertices + textures, see mesh.hpp)
    static std::map<std::string, Model> mModels;

    //All renderable objects
    std::vector<Renderable*> mRenderObjects;

    //All interactble objects (movement, collision, etc)
    std::vector<GameObject*> mInteractObjects;

    //Track all shader names
    std::vector<std::string> mShaderNames;

    //MVP matrix used for rendering
    glm::mat4 mMvp;

        //Functions
    //Constructor
    Game();

    //Load model into pool
    void loadModel(const std::string& modelName);

    //Read shader into ShaderManager
    void loadShader(const std::string& shaderName);

	//Debugging tool to display current list of shaders
	void printShaderPrograms() const;

	//Debugging tool to display current list of models
	void printModelNames() const;
};

#endif