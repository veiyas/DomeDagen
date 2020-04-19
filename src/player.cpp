#include "player.hpp"
#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<iostream>

#include"balljointconstraint.hpp"

Player::Player(const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation,
	           const std::string & name, float speed)
	: GameObject{ GameObject::PLAYER, radius, position, orientation },
	  GeometryHandler("player", objectModelName),
	  mName { name }, mPoints{ 0 }, mIsAlive{ true }, mSpeed{ speed }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());

	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();

	//Could probably be handled in a better way
	mConstraint = BallJointConstraint{ 170.f, 27.f };
}

Player::~Player()
{
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

void Player::update(float deltaTime)
{
	using namespace glm;

	float oldOrient = getOrientation();
	setOrientation(oldOrient + deltaTime * mTurnSpeed);

	glm::quat newPos = getPosition();
	newPos *= glm::quat(mSpeed * deltaTime * glm::vec3(cos(oldOrient), sin(oldOrient), 0.f));

	//Make sure player does not leave visible area
	mConstraint.apply(newPos);

	setPosition(glm::normalize(newPos));
}

void Player::render(const glm::mat4& mvp) const
{
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}