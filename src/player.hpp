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
};