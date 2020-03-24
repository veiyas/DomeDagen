#include "player.h"

#include "sgct/log.h"
#include <iostream>

Player::Player(const std::string& name /* position,  fler argument */)
    : name_{name}, points_{0}, is_alive_{true} {
  sgct::Log::Info("Player with name=\"%s\" created", name_.c_str());
}

Player::~Player() {
  sgct::Log::Info("Player with name=\"%s\" removed", name_.c_str());
}
