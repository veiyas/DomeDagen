
#include <iostream>

#include "player.hpp"
#include "game.hpp"
#include "sgct/log.h"
#include "glm/gtc/type_ptr.hpp"

Player::Player(const std::string& objType, float radius, const glm::quat& position, float orientation, const std::string& name)
	:	GameObject{ objType, radius, position, orientation }, mName { name }, mPoints{ 0 }, mIsAlive{ true }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());

	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("player");
	prg.bind();
	mMvpMatrixLoc = glGetUniformLocation(prg.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(prg.id(), "transformation");
	prg.unbind();
}

Player::~Player()
{
	sgct::Log::Info("Player with name=\"%s\" removed", mName.c_str());
}

void Player::update(float deltaTime)
{
	//glm::vec3 rotationAxis = glm::dot(tangent, normal);


	//glm::quat change;

	//glm::quat newPosition = change * getPosition() * glm::inverse(change);

	//setPosition(newPosition);
	//GameObject::update(deltaTime);
}

void Player::render() const
{
	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("player");
	prg.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTranformation()));
	this->renderModel();

	prg.unbind();
}
