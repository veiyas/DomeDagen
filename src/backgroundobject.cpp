#include "backgroundobject.hpp"

BackgroundObject::BackgroundObject()
	: GameObject{ GameObject::BACKGROUND, 1.0f, glm::quat(glm::vec3(0.49f, 1.0f, -1.0f)), 0.0f },
	GeometryHandler("background", "background")
{
	sgct::Log::Info("Background object created");
	setOrientation(getOrientation() + 1.2f);
	setShaderData();
}

BackgroundObject::~BackgroundObject()
{
	sgct::Log::Info("Background removed");
}

ObjectData BackgroundObject::getObjectData(bool isBackground) const
{
	ObjectData temp;

	//Positional data
	temp.mOrientation = getOrientation();
	temp.mRadius = getRadius();

	return temp;
}

void BackgroundObject::setObjectData(const ObjectData& newState)
{
	//Set data
	setOrientation(newState.mOrientation);
	setRadius(newState.mRadius);
}

void BackgroundObject::render(const glm::mat4& mvp, const glm::mat4& v) const
{
	if (!mEnabled)
		return;

	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}

void BackgroundObject::setShaderData()
{
	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();
}
