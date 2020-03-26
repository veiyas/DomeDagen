#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>

#include "gameobject.hpp"
#include "model.hpp"

class Player : public GameObject {
public:
	//No default ctor
	Player() = delete;

	//Ctor + Dtor
	Player(const unsigned objType, const glm::vec2 position, const float orientation, const std::string& name /* position, ... fler argument */);
	~Player();

	//Players should be unique
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	//Update position, (collision?) [Kanske göra i GameObject?]
	//void update(float deltaTime) override;

	//Render obejct
	void render() const override;

	//Accessors
	const int getPoints() const { return mPoints; };
	const bool isAlive() const { return mIsAlive; };
	const std::string& getName() const { return mName; };

	//Mutators
	void setPoints(int points) { mPoints = points; };
	void setIsAlive(bool isAlive) { mIsAlive = isAlive; };

private:
	//Player information/data
	int mPoints;
	bool mIsAlive;
	const std::string mName;
};

#endif