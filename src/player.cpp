#include "player.hpp"
#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<iostream>

#include"balljointconstraint.hpp"
Player::ColourSelector Player::mColourSelector = Player::ColourSelector{ };

Player::Player()
	: GameObject{ GameObject::PLAYER, 50.f, glm::quat(glm::vec3(0.f)), 0.f },
	  GeometryHandler("player", "diver"),
	  mName{ "temp" },
	  mPlayerColours{ mColourSelector.getNextPair() }
{
  mConstraint = BallJointConstraint{ 170.f, 27.f };
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const std::string name)
	: GameObject{ GameObject::PLAYER, 50.f, glm::quat(glm::vec3(0.f)), 0.f },
	  GeometryHandler("player", "diver"),
	  mName{ name },
	  mPlayerColours{ mColourSelector.getNextPair() }
{
  mConstraint = BallJointConstraint{ 170.f, 27.f };
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation,
	           const std::string & name, float speed)
	: GameObject{ GameObject::PLAYER, radius, position, orientation },
	  GeometryHandler("player", objectModelName),
	  mName { name },
	  mSpeed{ speed },
	  mPlayerColours{ mColourSelector.getNextPair() }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
  mConstraint = BallJointConstraint{ 170.f, 27.f };
	setShaderData();
}

Player::Player(const PlayerData& input)
	: GameObject{ GameObject::PLAYER, input.mRadius, glm::quat{}, input.mOrientation },
	  GeometryHandler("player", "diver"),
	  mName{ std::string(input.mNameLength, ' ') },
	  mPoints{ input.mPoints },
	  mIsAlive{ input.mIsAlive },
	  mSpeed{ input.mSpeed },
	  mPlayerColours{ mColourSelector.getNextPair() }
{
	//Copy new player name
	for (size_t i = 0; i < input.mNameLength; i++)
	{
		mName[i] = input.mPlayerName[i];
	}

	glm::quat temp{};
		temp.w = input.mW;
		temp.x = input.mX;
		temp.y = input.mY;
		temp.z = input.mZ;
	setPosition(temp);

	mShaderProgram.unbind();
	//Could probably be handled in a better way
	mConstraint = BallJointConstraint{ 170.f, 27.f };
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::~Player()
{
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

PlayerData Player::getPlayerData(bool isNewPlayer) const
{
	PlayerData temp;
	temp.mNewPlayer = isNewPlayer;
	temp.mNameLength = getName().length();

	//Positional data
	temp.mOrientation = getOrientation();
	temp.mRadius = getRadius();
	temp.mScale = getScale();
	temp.mSpeed = getSpeed();

	//Quat stuff
	temp.mW = getPosition().w;
	temp.mX = getPosition().x;
	temp.mY = getPosition().y;
	temp.mZ = getPosition().z;

	//Game state data
	temp.mPoints = getPoints();
	temp.mIsAlive = isAlive();
	temp.mEnabled = isEnabled();

	//Send name if this is a new player not present on nodes yet
	if (temp.mNewPlayer)
	{
		for (size_t i = 0; i < mName.length(); i++)
		{
			temp.mPlayerName[i] = mName.c_str()[i];
		}
	}

	return temp;
}

void Player::setPlayerData(const PlayerData& newState)
{
	//Position data
	setOrientation(newState.mOrientation);
	setRadius(newState.mRadius);
	setScale(newState.mScale);
	setSpeed(newState.mSpeed);

	//Quat stuff
	glm::quat newPosition;
		newPosition.w = newState.mW;
		newPosition.x = newState.mX;
		newPosition.y = newState.mY;
		newPosition.z = newState.mZ;
	setPosition(newPosition);

	//Game state data
	setPoints(newState.mPoints);
	setIsAlive(newState.mIsAlive);	
	setEnabled(newState.mEnabled);
}

void Player::update(float deltaTime)
{
  if (!mEnabled)
		return;  
	float oldOrient = getOrientation();
	setOrientation(oldOrient + deltaTime * mTurnSpeed);	

	glm::quat newPos = getPosition();
	newPos *= glm::quat(mSpeed * deltaTime * glm::vec3(cos(oldOrient), sin(oldOrient), 0.f));

	//Make sure player does not leave visible area
	mConstraint.apply(newPos);

	setPosition(glm::normalize(newPos));
}

void Player::render(const glm::mat4& mvp, const glm::mat4& v) const
{
	if (!mEnabled)
		return;

	mShaderProgram.bind();

	// frans; Even more color things!
	glUniform3fv(mPrimaryColLoc, 1, glm::value_ptr(mPlayerColours.first));
	glUniform3fv(mSecondaryColLoc, 1, glm::value_ptr(mPlayerColours.second));

	glm::vec3 cameraPos = glm::vec3((inverse(v))[3]);
	//std::cout << cameraPos.x << ' ' << cameraPos.y << ' ' << cameraPos.z << '\n';
	glUniform3fv(mCameraPosLoc, 1, glm::value_ptr(cameraPos));

	glm::mat4 transformation = getTransformation();
	glm::mat3 normalMatrix(glm::transpose(glm::inverse(transformation)));

	glUniformMatrix3fv(mNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(transformation));
	this->renderModel();

	//Bone stuff
	auto transforms = mModel.getTransforms(); //TODO handle this in a way that avoids copying
	if (transforms.size() != 0)
		std::cout << "yay!\n";

	glUniformMatrix4fv(mBonesLoc, transforms.size(), GL_FALSE, reinterpret_cast<GLfloat *>(transforms.data()));

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

void Player::setShaderData()
{
	mShaderProgram.bind();

	mMvpMatrixLoc    = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc  = glGetUniformLocation(mShaderProgram.id(), "transformation");
	mViewMatrixLoc   = glGetUniformLocation(mShaderProgram.id(), "view");
	mCameraPosLoc    = glGetUniformLocation(mShaderProgram.id(), "cameraPos");
	mNormalMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "normalMatrix");

	mBonesLoc = glGetUniformLocation(mShaderProgram.id(), "bones");

	// frans; More color things
	mPrimaryColLoc = glGetUniformLocation(mShaderProgram.id(), "primaryCol");
	mSecondaryColLoc = glGetUniformLocation(mShaderProgram.id(), "secondaryCol");

	mShaderProgram.unbind();
}
