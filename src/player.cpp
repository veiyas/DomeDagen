#include "player.hpp"
#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<iostream>

#include"balljointconstraint.hpp"

const float Player::mFOV = 170.0f;
const float Player::mTILT = 27.0f;

Player::ColourSelector Player::mColourSelector = Player::ColourSelector{ };

Player::Player()
	: GameObject{ GameObject::PLAYER, 50.f, glm::quat(glm::vec3(0.f)), 0.f, PLAYERSCALE },
	  GeometryHandler("player", "diver"),
	  mName{ "temp" },
	  mPlayerColours{ mColourSelector.getNextPair() },
	  mConstraint{ mFOV, mTILT }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const std::string name)
	: GameObject{ GameObject::PLAYER, 50.f, glm::quat(glm::vec3(0.f)), 0.f, PLAYERSCALE },
	  GeometryHandler("player", "diver"),
	  mName{ name },
	  mPlayerColours{ mColourSelector.getNextPair() },
	  mConstraint{ mFOV, mTILT }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation,
	           const std::string & name, float speed)
	: GameObject{ GameObject::PLAYER, radius, position, orientation, PLAYERSCALE },
	  GeometryHandler("player", objectModelName),
	  mName { name },
	  mSpeed{ speed },
	  mPlayerColours{ mColourSelector.getNextPair() },
	  mConstraint{ mFOV, mTILT }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const PlayerData& newPlayerData,
	const PositionData& newPosData)
	: GameObject{ GameObject::PLAYER, newPosData.mRadius, glm::quat{}, 0.f, PLAYERSCALE },
	GeometryHandler("player", "diver"),
	mName{ std::string(newPlayerData.mNameLength, ' ') },
	mPoints{ newPlayerData.mPoints },
	mIsAlive{ newPlayerData.mIsAlive },
	mSpeed{ newPlayerData.mSpeed },
	mConstraint{ mFOV, mTILT }
{
	//Copy new player name
	for (size_t i = 0; i < newPlayerData.mNameLength; i++)
	{
		mName[i] = newPlayerData.mPlayerName[i];
	}

	glm::quat temp{};
		temp.w = newPosData.mW;
		temp.x = newPosData.mX;
		temp.y = newPosData.mY;
		temp.z = newPosData.mZ;
	setPosition(temp);

	mShaderProgram.unbind();
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());	

	auto& col = newPlayerData.mPlayerColours;
	mPlayerColours = std::make_pair(glm::vec3(col.mR1, col.mG1, col.mB1),
	                                glm::vec3(col.mR2, col.mG2, col.mB2));
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

	//Game state data
	temp.mPoints = getPoints();
	temp.mIsAlive = isAlive();
	temp.mEnabled = isEnabled();
	temp.mSpeed = getSpeed();

	//Color data
	glm::vec3 color1 = mPlayerColours.first;
		temp.mPlayerColours.mR1 = color1.r;
		temp.mPlayerColours.mG1 = color1.g;
		temp.mPlayerColours.mB1 = color1.b;
	glm::vec3 color2 = mPlayerColours.second;
		temp.mPlayerColours.mR2 = color2.r;
		temp.mPlayerColours.mG2 = color2.g;
		temp.mPlayerColours.mB2 = color2.b;

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

void Player::setPlayerData(const PlayerData& newPlayerData, const PositionData& newPosData)
{
	//Position data
	setOrientation(newPosData.mOrientation);
	setRadius(newPosData.mRadius);
	setScale(newPosData.mScale);

	//Quat stuff
	glm::quat newPosition;
	newPosition.w = newPosData.mW;
	newPosition.x = newPosData.mX;
	newPosition.y = newPosData.mY;
	newPosition.z = newPosData.mZ;
	setPosition(newPosition);

	//Game state data
	setSpeed(newPlayerData.mSpeed);
	setPoints(newPlayerData.mPoints);
	setIsAlive(newPlayerData.mIsAlive);
	setEnabled(newPlayerData.mEnabled);
	setSpeed(newPlayerData.mSpeed);
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

	mShaderProgram.unbind();
}

void Player::setShaderData()
{
	GeometryHandler::setShaderData();
	// frans; More color things
	mPrimaryColLoc = glGetUniformLocation(mShaderProgram.id(), "primaryCol");
	mSecondaryColLoc = glGetUniformLocation(mShaderProgram.id(), "secondaryCol");
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