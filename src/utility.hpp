#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <tuple>
#include <sstream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "model.hpp"
#include "constants.hpp"

class Utility
{
public:
	static std::string findRootDir();

	static std::tuple<unsigned, std::string> getNewPlayerData(std::istringstream& input);

	static std::tuple<unsigned int, float> getTurnSpeed(std::istringstream& input);

	static unsigned int textureFromFile(const char* path, const std::string& directory/*bool gamma = false*/);

	static glm::mat4 aiMatrixToGlm(const aiMatrix4x4& in);
private:
	
};