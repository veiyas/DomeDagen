#include "game.hpp"
#include "utility.hpp"

Game* Game::mInstance = nullptr;

Game& Game::getInstance()
{
	if (!mInstance) {
		mInstance = new Game{};
	}
	return *mInstance;
}

void Game::printShaderPrograms()
{
	std::cout << sgct::ShaderManager::instance().shaderProgram("player").name();
}

Game::Game()
{
	readShader("player");
	readShader("testing");
}

void Game::readShader(const std::string& shader_name)
{
	//Define path and strings to hold shaders
	std::string path = Utility::findRootDir() + "/src/shaders/" + shader_name;
	std::string vert = "";
	std::string frag = "";

	//Read shaders into strings
	std::ifstream in_vert{ path + "vert.glsl" };
	std::ifstream in_frag{ path + "frag.glsl" };
	
	if (in_vert.good() && in_frag.good()) {
		vert = std::string(std::istreambuf_iterator<char>(in_vert), {});
		frag = std::string(std::istreambuf_iterator<char>(in_frag), {});
	}
	else
	{
		std::cout << "ERROR OPENING SHADER FILE" + shader_name;
	}
	in_vert.close(); in_frag.close();

	sgct::ShaderManager::instance().addShaderProgram(shader_name, vert, frag);
}
