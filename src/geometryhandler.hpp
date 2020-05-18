#pragma once

#include "modelmanager.hpp"

//This class is privately inherited to classes needing models and accompanied functionality
class GeometryHandler
{
public:
	GeometryHandler(const std::string& shaderProgramName, const std::string& objectModelName)
		:mShaderProgram{sgct::ShaderManager::instance().shaderProgram(shaderProgramName)},
		 mModel{&ModelManager::instance().getModel(objectModelName)} {}

	GeometryHandler(const GeometryHandler&) = default;
	GeometryHandler(GeometryHandler&&) = default;
	GeometryHandler& operator=(const GeometryHandler&) = delete;
	GeometryHandler& operator=(GeometryHandler&& src) noexcept
	{
		std::swap(mTransMatrixLoc, src.mTransMatrixLoc);
		std::swap(mMvpMatrixLoc, src.mMvpMatrixLoc);
		std::swap(mModel, src.mModel);
		return *this;
	}
	//Do not want to remove mModel as this removes the model for all objects
	~GeometryHandler() { mModel = nullptr; }
	
	//Shader matrix locations
	GLint mTransMatrixLoc = -1;
	GLint mMvpMatrixLoc = -1;
	GLint mViewMatrixLoc = -1;
	GLint mNormalMatrixLoc = -1;
	GLint mCameraPosLoc = -1;

	//Reference to shader in shader pool
	const sgct::ShaderProgram& mShaderProgram;

	//POINTER to model in model pool (references are not swappable)
	//Needs to be swappable for collectible pooling
	Model* mModel;

	//Render model
	void renderModel() const { mModel->render(); };

	//Set shader data
	void setShaderData()
	{
		mShaderProgram.bind();

		mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
		mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");
		mViewMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "view");
		mCameraPosLoc = glGetUniformLocation(mShaderProgram.id(), "cameraPos");
		mNormalMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "normalMatrix");

		mShaderProgram.unbind();
	}
};