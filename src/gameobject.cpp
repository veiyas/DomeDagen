#include "gameobject.hpp"

#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<glm/gtc/constants.hpp>

#include<iostream>

GameObject& GameObject::operator=(GameObject&& src) noexcept
{
	std::swap(mPosition, src.mPosition);
	std::swap(mRadius, src.mRadius);
	std::swap(mOrientation, src.mOrientation);
	std::swap(mScale, src.mScale);

	return *this;
}

GameObject::GameObject(const unsigned objType, float radius, const glm::quat& position, float orientation, float scale)
	: mRadius{ radius }, mPosition{ position }, mOrientation{ orientation },
	 mScale{ scale }, mObjType{ objType }
{

}

glm::mat4 GameObject::getTransformation() const
{
	glm::mat4 trans  = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -mRadius));
	glm::mat4 orient = glm::rotate(glm::mat4(1.f), mOrientation, glm::vec3(0, 0, 1));
	glm::mat4 rot    = glm::toMat4(mPosition);
	glm::mat4 scale  = glm::scale(glm::mat4(1.f), glm::vec3(mScale));

	//std::cout << glm::to_string(trans) << '\n';

	//TODO Put model rotation in a variable to allow models with different orientation
	return rot * trans * orient * scale
		* glm::rotate(glm::mat4(1.f), -glm::half_pi<float>(), glm::vec3(0.f, 0.f, 1.f))
		* glm::rotate(glm::mat4(1.f), glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
}

const PositionData GameObject::getPositionData() const
{
	PositionData temp;
	//Positional data
	temp.mOrientation = getOrientation();
	temp.mRadius = getRadius();
	temp.mScale = getScale();
	//temp.mSpeed = getSpeed();

	//Quat stuff
	temp.mW = getPosition().w;
	temp.mX = getPosition().x;
	temp.mY = getPosition().y;
	temp.mZ = getPosition().z;

	return temp;
}

void GameObject::setPositionData(const PositionData& newPosition)
{
	//Position data
	setOrientation(newPosition.mOrientation);
	setRadius(newPosition.mRadius);
	setScale(newPosition.mScale);

	//Quat stuff
	glm::quat newQuat;
	newQuat.w = newPosition.mW;
	newQuat.x = newPosition.mX;
	newQuat.y = newPosition.mY;
	newQuat.z = newPosition.mZ;
	setPosition(newQuat);
}
