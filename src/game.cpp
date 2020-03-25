#include "game.hpp"
#include "utility.hpp"
#include "sgct/log.h"

//Define instance
Game* Game::mInstance = nullptr;

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
	delete mInstance;
}

void Game::printShaderPrograms() const
{
	sgct::Log::Info(sgct::ShaderManager::instance().shaderProgram("player").name().c_str());
	sgct::Log::Info(sgct::ShaderManager::instance().shaderProgram("player").name().c_str());
}

Game::Game()
{
	readShader("player");
	readShader("testing");
}

void Game::readShader(const std::string& shaderName)
{
	//Define path and strings to hold shaders
	std::string path = Utility::findRootDir() + "/src/shaders/" + shaderName;
	std::string vert = "";
	std::string frag = "";

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
		std::cout << "ERROR OPENING SHADER FILE: " + shaderName;
	}
	in_vert.close(); in_frag.close();

	sgct::ShaderManager::instance().addShaderProgram(shaderName, vert, frag);
}
