#include "player.hpp"

Player::Player(const unsigned objectType, const std::string& objectModelName,
	const glm::vec3 position, const float orientation, const std::string playerName)
	: GameObject{ objectType, position, orientation }, GeometryHandler("player", objectModelName),
	  mName{playerName}, mIsAlive{true}, mPoints{0}
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();
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
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
	this->renderModel();

	mShaderProgram.unbind();
}
