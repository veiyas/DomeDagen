#pragma once

#include <string>
#include <iostream>
#include <tuple>

#include "sgct/log.h"

#include "gameobject.hpp"
#include "geometryhandler.hpp"

//POD struct to encode/decode game state data
//Ctor, initialisation disallowed to KEEP IT POD
constexpr unsigned NAMELIMIT = 20;

struct PlayerData
{
public:
	//Position data
	float mRadius;
	float mOrientation;
	float mScale;
	float mSpeed;

	//Quat stuff
	float mW;
	float mX;
	float mY;
	float mZ;

	//Game state data
	int   mPoints;
	bool  mEnabled;
	bool  mIsAlive;

	//These are only used when a new player is created on nodes
	//C-style array because of POD
	char mPlayerName[NAMELIMIT];
	bool mNewPlayer;
	unsigned mNameLength;
};

class Player : public GameObject, private GeometryHandler
{
public:
	//Default ctor used for debugging
	Player();

	//Used for creating from tuple (server requested)
	Player(const std::string name);

	//Big ctor
	Player(const std::string & objectModelName, float radius, 
		   const glm::quat& position, float orientation,
		   const std::string& name, float speed);

	//Ctor from positiondata (syncing new players on nodes)
	Player(const PlayerData& input);

	//Dtor
	~Player();

	//Players should be unique
	Player(const Player&) = default;
	Player& operator=(const Player&) = delete;

	//Get/set playerdata during synchronisation
	PlayerData getPlayerData(bool isNewPlayer) const;
	void setPlayerData(const PlayerData& newState);

	//Update position
	void update(float deltaTime) override;

	//Render obejct
	void render(const glm::mat4& mvp) const override;

	//Activator + deactivator	
	void enablePlayer() { mEnabled = true; }
	void disablePlayer() { mEnabled = false; }

	//Accessors
	float getSpeed() const { return mSpeed; };
	float getTurnSpeed() const { return mTurnSpeed; }
	const int getPoints() const { return mPoints; };
	const bool isAlive() const { return mIsAlive; };
	const bool isEnabled() const { return mEnabled; };
	const std::string& getName() const { return mName; };

	//Mutators
	void setEnabled(bool state) { mEnabled = state; }
	void setSpeed(float speed) override { mSpeed = speed; };
	void setPoints(int points) { mPoints = points; };
	void setIsAlive(bool isAlive) { mIsAlive = isAlive; };
	void setTurnSpeed(float turnSpeed) override { mTurnSpeed = turnSpeed; };

private:
	//Player information/data
	float mTurnSpeed = 0.2f;
	int   mPoints;
	bool  mIsAlive;
	float mSpeed;
	std::string mName;

	//If person disconnect/reconnect
	bool mEnabled = true;

	//Set shader data
	void setShaderData();
};