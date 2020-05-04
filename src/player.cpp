#include "player.hpp"
#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<iostream>

Player::Player()
	: GameObject{ GameObject::PLAYER, 50.f, glm::quat(glm::vec3(0.f)), 0.f },
	GeometryHandler("player", "fish"),
	mName{ "temp" }, mPoints{ 0 }, mIsAlive{ true }, mSpeed{ 0.5f }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const std::string name)
	: GameObject{ GameObject::PLAYER, 50.f, glm::quat(glm::vec3(0.f)), 0.f },
	GeometryHandler("player", "fish"),
	mName{ name }, mPoints{ 0 }, mIsAlive{ true }, mSpeed{ 0.5f }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

Player::Player(const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation,
	           const std::string & name, float speed)
	: GameObject{ GameObject::PLAYER, radius, position, orientation },
	  GeometryHandler("player", objectModelName),
	  mName { name }, mPoints{ 0 }, mIsAlive{ true }, mSpeed{ speed }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	setShaderData();
}

//TODO fix this to work with SyncableData
Player::Player(const PlayerData& newPlayerData,
	const PositionData& newPosData)
	: GameObject{ GameObject::PLAYER, newPosData.mRadius, glm::quat{}, 0.f },
	GeometryHandler("player", "fish"),
	mName{ std::string(newPlayerData.mNameLength, ' ') }, mPoints{ newPlayerData.mPoints }, mIsAlive{ newPlayerData.mIsAlive },
	mSpeed{ newPlayerData.mSpeed }
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

	//Game state data
	temp.mPoints = getPoints();
	temp.mIsAlive = isAlive();
	temp.mEnabled = isEnabled();
	temp.mSpeed = getSpeed();

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

void Player::render(const glm::mat4& mvp) const
{
	if (!mEnabled)
		return;

	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}

void Player::setShaderData()
{
	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();
}
