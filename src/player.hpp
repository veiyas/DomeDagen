#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>

#include "gameobject.hpp"

class Player : public GameObject {
public:
	Player() = delete;
	Player(const std::string& name /* position, ... fler argument */);
	virtual ~Player();

	// Players should be unique
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void update(float delta_time) override;

	// accessors
	int getPoints() const { return mPoints; };
	bool isAlive() const { return mIsAlive; };
	const std::string& getName() const { return mName; };

private:
	// l�gg till n�t id eller ip typ eller b�da

	int mPoints;
	bool mIsAlive;
	const std::string mName;  // vad h�nder om namn ej �r unikt? b�r kanske
							  // hanteras i webbservern
};

#endif