#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>

#include "gameobject.h"

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
	int getPoints() const { return points_; };
	bool isAlive() const { return is_alive_; };
	const std::string& getName() const { return name_; };

private:
	// lägg till nåt id eller ip typ eller båda

	int points_;
	bool alive_;
	const std::string name_;  // vad händer om namn ej är unikt? bör kanske
							  // hanteras i webbservern
};

#endif