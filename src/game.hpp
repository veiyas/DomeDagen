#ifndef GAME_H_
#define GAME_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glm/matrix.hpp"
#include "renderable.hpp"
#include "gameobject.hpp"


//Implemented as singleton
class Game
{
public:
    //Get instance
    static Game& getInstance();

    //Destroy instance
    static void destroy();

    //Copying forbidden
    Game(Game const&) = delete;
    void operator=(Game const&) = delete;

    //Debugging tool to display current list of shaders
    void printShaderPrograms() const;

    //Render objects
    void render() const;

    //Set MVP matrix
    void setMVP(const glm::mat4& mvp);

private:
    //Constructor
    Game();

    //Read shader into ShaderManager
    void readShader(const std::string& shaderName);
    
    //Singleton instance of game
    static Game* mInstance;

    //All renderable objects
    std::vector<Renderable> mRenderObjects;

    //All interactble objects (movement, collision, etc)
    std::vector<GameObject> mInteractObjects;

    //MVP matrix used for rendering
    glm::mat4 mMvp;
};

#endif