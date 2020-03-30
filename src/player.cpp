
#include <iostream>

#include "player.hpp"
#include "game.hpp"
#include "sgct/log.h"
#include "glm/gtc/type_ptr.hpp"

Player::Player(const std::string& objType, const glm::vec3 position, const float orientation, const std::string& name /* position, ... fler argument */)
	:	GameObject{ objType, position, orientation }, mName { name }, mPoints{ 0 }, mIsAlive{ true }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());

	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("player");
	prg.bind();
	mMvpMatrixLoc = glGetUniformLocation(prg.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(prg.id(), "transformation");
	prg.unbind();
}

Player::~Player() {
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

//void Player::update(float deltaTime) const {
	//velocity_ = deltaTime * acceleration_;  // funkar nog ej bra just f�r v�rt spel

	//GameObject::update(deltaTime);
//}

void Player::render() const
{
	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("player");
	prg.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTranformation()));
	this->renderModel();

	prg.unbind();
}
