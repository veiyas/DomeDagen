#include "player.hpp"

Player::Player(const unsigned objectType, const std::string & objectModelName, float radius,
	           const glm::quat & position, float orientation, const std::string & name)
	: GameObject{ objectType, radius, position, orientation }, GeometryHandler("player", objectModelName),
	  mName { name }, mPoints{ 0 }, mIsAlive{ true }
{
	sgct::Log::Info("Player with name=\"%s\" created", mName.c_str());
	mShaderProgram.bind();

	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");

	mShaderProgram.unbind();
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
	mShaderProgram.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
	this->renderModel();

	mShaderProgram.unbind();
}
