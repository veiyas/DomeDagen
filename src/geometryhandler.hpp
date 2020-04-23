#pragma once

#include "modelmanager.hpp"

//This class is privately inherited to classes needing models and accompanied functionality
class GeometryHandler
{
public:
	GeometryHandler(const std::string& shaderProgramName, const std::string& objectModelName)
		:mShaderProgram{sgct::ShaderManager::instance().shaderProgram(shaderProgramName)},
		 mModel{ModelManager::instance().getModel(objectModelName)} {}

	//Shader matrix locations
	GLint mTransMatrixLoc = -1;
	GLint mMvpMatrixLoc = -1;
	GLint mViewMatrixLoc = -1;

	GLint mCameraPosLoc = -1;

	//Reference to shader in shader pool
	const sgct::ShaderProgram& mShaderProgram;

	//Reference to model in model pool
	const Model& mModel;

	//Render model
	void renderModel() const { mModel.render(); };
};