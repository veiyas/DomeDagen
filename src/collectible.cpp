#include "collectible.hpp"

Collectible::Collectible(const std::string objectModelName)
	:GameObject{ GameObject::COLLECTIBLE, 50.f, glm::vec3(1.f, 0.f, 0.f), 0.f }
	, GeometryHandler{ "collectible", objectModelName }, mSpeed{0.1f}
{
	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");
}

void Collectible::render(const glm::mat4& mvp) const
{
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}