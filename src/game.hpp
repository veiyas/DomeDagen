#ifndef GAME_H_
#define GAME_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "renderable.hpp"

//Implemented as singleton
class Game
{
public:
    //Retrieve instance of the singleton
    static Game& getInstance();

    //Copying forbidden
    Game(Game const&) = delete;
    void operator=(Game const&) = delete;

    //Debugging tool to display current list of shaders
    void printShaderPrograms();

private:
    //ctor and singleton instance
    Game();
    static Game* mInstance;

    void readShader(const std::string& shaderName);
    std::vector<Renderable> mRenderableObjects;
};

#endif