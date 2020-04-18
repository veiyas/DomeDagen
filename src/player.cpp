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

	//FIXME put this where it should be. Use shared ptr?
	mConstraint = BallJointConstraint{ 160.f, 27.f };
}

Player::~Player()
{
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

void Player::update(float deltaTime)
{
	using namespace glm;

	//Update orientation
	setOrientation(getOrientation() + deltaTime * mTurnSpeed);

	//Update position on sphere
	glm::quat newPos = getPosition();
	newPos *= glm::quat(
		mSpeed * deltaTime * glm::vec3(cos(getOrientation()), sin(getOrientation()), 0.f)
	);


	mConstraint.clamp(newPos);


	//glm::vec3 mPlanePos = glm::vec3(0.f, 0.f, -0.1f);
	//glm::vec3 mZ = glm::vec3(0.f, 0.f, -1.f); // ????
	///////////////////////////////////
	//using namespace glm;


	////Direction of q
	//const vec3 direction = newPos * vec3(0.f, 0.f, -1.f);

	////Vector from ??? to direction
	//const vec3 relation = direction - mPlanePos;

	//const float dotProd = dot(relation, mPlanePos);
	////sgct::Log::Info("%d", dotProd);

	//if (dotProd < 0)
	//	return;


	setPosition(glm::normalize(newPos)); //Normalize might not be necessary?
}

void Player::render(const glm::mat4& mvp) const
{
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}