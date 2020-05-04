#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <algorithm>
#include <random>
#include <chrono>

#include "sgct/log.h"

#include "gameobject.hpp"
#include "geometryhandler.hpp"
#include "balljointconstraint.hpp"

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
	void render(const glm::mat4& mvp, const glm::mat4& v) const override;

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
    
    // Iris: trying to send colours
    std::pair<glm::vec3, glm::vec3> getColours() const { return mPlayerColours; };

	//Mutators
	void setEnabled(bool state) { mEnabled = state; }
	void setSpeed(float speed) override { mSpeed = speed; };
	void setPoints(int points) { mPoints = points; };
	void setIsAlive(bool isAlive) { mIsAlive = isAlive; };
	void setTurnSpeed(float turnSpeed) override { mTurnSpeed = turnSpeed; };

private:
	//Player information/data
	float mTurnSpeed = 0.2f;
	BallJointConstraint mConstraint;
	int   mPoints    = 0;
	bool  mIsAlive   = true;
	float mSpeed     = 0.2f;
	std::string mName;

	// frans; Trying something with colors
	const std::pair<glm::vec3, glm::vec3> mPlayerColours;
	GLint mPrimaryColLoc;
	GLint mSecondaryColLoc;

	struct ColourSelector
	{
		ColourSelector();

		std::vector<glm::vec3> mPrimaryColours{
			{0.2f, 0.2f, 0.2f},		// Dark gray
			{0.3f, 0.3f, 0.5f},		// Navy blue
			{0.8f, 0.9f, 0.9f},		// Pale sky blue
			{0.1f, 0.3f, 0.3f},		// Dark green
			{0.5f, 0.2f, 0.2f},		// Brown
			{0.9f, 0.9f, 0.8f},		// Beige
			{0.3f, 0.2f, 0.3f},		// Mauve
			{0.4f, 0.1f, 0.2f},		// Wine red
		};
		std::vector<glm::vec3> mSecondaryColours{
			{1.f, 0.2f, 0.2f},		// Red
			{1.f, 0.4f, 0.8f},		// Pink
			{0.4f, 0.8f, 1.f},		// Cyan
			{1.f, 1.f, 0.1f},		// Yellow
			{0.4f, 1.f, 0.2f},		// Green
			{1.f, 0.7f, 0.2f},		// Orange
			{0.8f, 0.6f, 1.f},		// Lavender
			{0.8f, 0.8f, 0.8f}		// Silver
		};

		std::vector<glm::vec3>::iterator mPrimaryIt;
		std::vector<glm::vec3>::iterator mSecondaryIt;

		std::pair<glm::vec3, glm::vec3> getNextPair();
		void shuffle();
		void reset();
	};
	static ColourSelector mColourSelector;

	//If person disconnect/reconnect
	bool mEnabled = true;

	//Set shader data
	void setShaderData();
};
