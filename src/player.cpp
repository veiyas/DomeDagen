#include "player.hpp"
#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<iostream>

Player::Player(const unsigned objectType, const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation, const std::string & name)
	: GameObject{ objectType, radius, position, orientation }, GeometryHandler("player", objectModelName),
	  mName { name }, mPoints{ 0 }, mIsAlive{ true }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());

	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();
}

Player::~Player()
{
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

void Player::update(float deltaTime)
{
	glm::quat newPos = getPosition();
	//TODO Make framerate independent, use speed, etc.
	newPos *= glm::quat(0.001f * glm::vec3(cos(getOrientation()), sin(getOrientation()), 0.f));

	setPosition(glm::normalize(newPos));

	//TODO Constrain to visible area

	//sgct::Log::Info("Updatedd");
}

void Player::render() const
{
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}
