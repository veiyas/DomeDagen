#include "utility.hpp"

std::string Utility::findRootDir()
{
	auto searcher = std::filesystem::current_path();
	while (searcher.has_parent_path())
	{
		if (searcher.filename() == "DomeDagen" || searcher.filename() == "Domedagen" || searcher.filename() == "domedagen")
			return searcher.string();
		else
			searcher = searcher.parent_path();
	}
	return "";
}

std::tuple<unsigned, std::string> Utility::getNewPlayerData(std::istringstream& input)
{
	unsigned int id;
	std::string name;

	input >> id;
	input >> name;

	return std::make_tuple(id, name);
}

std::tuple<unsigned int, float> Utility::getTurnSpeed(std::istringstream& input)
{
	unsigned int id;
	float rotAngle;

	input >> id;
	input >> rotAngle;

	return std::make_tuple(id, rotAngle);
}

unsigned int Utility::textureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGBA;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}