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
	Player(const std::string& objType, float radius, const glm::quat& position, float orientation, const std::string& name);
	~Player();

	//Players should be unique
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	//Update position, (collision?) [Kanske göra i GameObject?]
	void update(float deltaTime) override;

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

	//Shader matrix locations and tranformation matrix
	GLint mTransMatrixLoc = -1;
	GLint mMvpMatrixLoc = -1;
};

#endif