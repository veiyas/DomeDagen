#pragma once

#include "game.hpp"

//This class is privately inherited to classes needing models and accompanied functionality
class GeometryHandler
{
public:
	GeometryHandler(const std::string& shaderProgramName, const std::string& objectModelName)
		:mShaderProgram{sgct::ShaderManager::instance().shaderProgram(shaderProgramName)},
		 mModel{Game::getInstance().getModel(objectModelName)},
		 mTransformation{glm::mat4(1.f)}{}

	//Shader matrix locations
	GLint mTransMatrixLoc = -1;
	GLint mMvpMatrixLoc = -1;

	//Objects current tranformation
	glm::mat4 mTransformation;

	//Reference to shader in shader pool
	const sgct::ShaderProgram& mShaderProgram;

	//Reference to model in model pool
	const Model& mModel;

	//Render model
	void renderModel() const { mModel.render(); };

	//Get current transformation matrix TODO IMPLEMENT QUAT SUPPORT
	glm::mat4 getTransformation() { return mTransformation; };
};