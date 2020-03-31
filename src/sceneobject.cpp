#include "sceneobject.hpp"

SceneObject::SceneObject(const unsigned objectType, const std::string & objectModelName,
	                     float radius, const glm::quat& position, const float orientation)
	: GameObject{ objectType, radius, position, orientation }, GeometryHandler("sceneobject", objectModelName)
{
	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();
}

void SceneObject::render() const
{
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}