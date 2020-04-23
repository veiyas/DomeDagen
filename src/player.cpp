#include "player.hpp"
#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<iostream>

Player::ColourSelector Player::mColourSelector = Player::ColourSelector{ };

Player::Player(const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation,
	           const std::string & name, float speed)
	: GameObject{ GameObject::PLAYER, radius, position, orientation },
	  GeometryHandler("player", objectModelName),
	  mName { name }, mPoints{ 0 }, mIsAlive{ true }, mSpeed{ speed },
	  mPlayerColours{ mColourSelector.getNextPair() }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());

	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");
	mViewMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "view");
	
	mCameraPosLoc = glGetUniformLocation(mShaderProgram.id(), "cameraPos");

	// frans; More color things
	mPrimaryColLoc = glGetUniformLocation(mShaderProgram.id(), "primaryCol");
	mSecondaryColLoc = glGetUniformLocation(mShaderProgram.id(), "secondaryCol");

	mShaderProgram.unbind();
}

Player::~Player()
{
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

void Player::update(float deltaTime)
{
	//Update orientation
	setOrientation(getOrientation() + deltaTime * mTurnSpeed);

	//Update position on sphere
	glm::quat newPos = getPosition();
	newPos *= glm::quat(
		mSpeed * deltaTime * glm::vec3(cos(getOrientation()), sin(getOrientation()), 0.f)
	);
	setPosition(glm::normalize(newPos)); //Normalize might not be necessary?

	//TODO Constrain to visible area
}

void Player::render(const glm::mat4& mvp, const glm::mat4& v) const
{
	mShaderProgram.bind();

	// frans; Even more color things!
	glUniform3fv(mPrimaryColLoc, 1, glm::value_ptr(mPlayerColours.first));
	glUniform3fv(mSecondaryColLoc, 1, glm::value_ptr(mPlayerColours.second));

	glm::vec3 cameraPos = glm::vec3((inverse(v))[3]);
	//std::cout << cameraPos.x << ' ' << cameraPos.y << ' ' << cameraPos.z << '\n';
	glUniform3fv(mCameraPosLoc, 1, glm::value_ptr(cameraPos));

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}

Player::ColourSelector::ColourSelector()
{
	shuffle();
}

std::pair<glm::vec3, glm::vec3> Player::ColourSelector::getNextPair()
{
	if (mPrimaryIt == mPrimaryColours.end())
	{
		mPrimaryIt = mPrimaryColours.begin();
	}

	if (mSecondaryIt == mSecondaryColours.end())
	{
		mSecondaryIt = mSecondaryColours.begin();
		++mPrimaryIt;
	}

	return std::pair<glm::vec3, glm::vec3>(*mPrimaryIt, *mSecondaryIt++);
}

void Player::ColourSelector::shuffle()
{
	std::default_random_engine rng;
	rng.seed(std::chrono::system_clock::now().time_since_epoch().count());

	std::shuffle(mPrimaryColours.begin(), mPrimaryColours.end(), rng);
	std::shuffle(mSecondaryColours.begin(), mSecondaryColours.end(), rng);

	reset();
}

void Player::ColourSelector::reset()
{
	mPrimaryIt = mPrimaryColours.begin();
	mSecondaryIt = mSecondaryColours.begin();
}