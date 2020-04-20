#pragma once

#include <string>
#include <iostream>
#include <tuple>

#include "sgct/log.h"

#include "gameobject.hpp"
#include "geometryhandler.hpp"

#include "balljointconstraint.hpp"

class Player : public GameObject, private GeometryHandler
{
public:
	//No default ctor
	Player() = delete;

	//Ctor + Dtor
	Player(const std::string & objectModelName, float radius, 
		   const glm::quat& position, float orientation,
		   const std::string& name, float speed);

	~Player();

	//Players should be unique
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	//Update position, (collision?) [Kanske göra i GameObject?]
	void update(float deltaTime) override;

	//Render obejct
	void render(const glm::mat4& mvp) const override;

	//Accessors
	float getSpeed() const { return mSpeed; };
	float getTurnSpeed() const { return mTurnSpeed; }
	const int getPoints() const { return mPoints; };
	const bool isAlive() const { return mIsAlive; };
	const std::string& getName() const { return mName; };

	//Mutators
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
	const std::string mName;

	BallJointConstraint mConstraint;
};