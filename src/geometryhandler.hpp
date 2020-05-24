#pragma once

#include "modelmanager.hpp"

//This class is privately inherited to classes needing models and accompanied functionality
//This class is also very unorganized
class GeometryHandler
{
public:
	GeometryHandler(const std::string& shaderProgramName, const std::string& objectModelName)
		:mShaderProgram{ sgct::ShaderManager::instance().shaderProgram(shaderProgramName) },
		mModel{ &ModelManager::instance().getModel(objectModelName) },
		mModelSlot{ ModelManager::instance().findModelSpot(objectModelName)} {}

	GeometryHandler(const GeometryHandler&) = default;
	GeometryHandler(GeometryHandler&&) = default;
	GeometryHandler& operator=(const GeometryHandler&) = delete;
	GeometryHandler& operator=(GeometryHandler&& src) noexcept
	{
		std::swap(mTransMatrixLoc, src.mTransMatrixLoc);
		std::swap(mMvpMatrixLoc, src.mMvpMatrixLoc);
		std::swap(mModel, src.mModel);
		std::swap(mModelSlot, src.mModelSlot);
		return *this;
	}
	//Do not remove mModel as this removes the model for all objects
	~GeometryHandler() { mModel = nullptr; }

	//Get model pointer index
	int getModelPointerIndex(){return mModelSlot;}

	//Set new model from slot index in ModelManager
	void setModelFromInt(const int index)
	{mModel = &ModelManager::instance().getModel(index);}
	
	//Shader matrix locations
	GLint mTransMatrixLoc = -1;
	GLint mMvpMatrixLoc = -1;
	GLint mViewMatrixLoc = -1;
	GLint mNormalMatrixLoc = -1;
	GLint mCameraPosLoc = -1;

	//Reference to shader in shader pool
	const sgct::ShaderProgram& mShaderProgram;
	const sgct::ShaderProgram& getShader() { return mShaderProgram; }

	//POINTER to model in model pool (references are not swappable)
	//Needs to be swappable for collectible pooling
	Model* mModel;
	int mModelSlot = -1;

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