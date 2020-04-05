#include "gameobject.hpp"

#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>
#include<glm/gtc/constants.hpp>

#include<iostream>

GameObject::GameObject(const unsigned objType, float radius, const glm::quat& position, float orientation)
	: mRadius{ radius }, mPosition{ position }, mOrientation{ orientation },
	 mScale{ 10.f }, mObjType{ objType }
{

}

PositionData GameObject::getMovementData(unsigned int id)
{
	PositionData temp;
	
	temp.mId = id;

	//Positional stuff
	temp.mOrientation = getOrientation();
	temp.mRadius = getRadius();
	temp.mScale = getScale();

	temp.mW = getPosition().w;
	temp.mX = getPosition().x;
	temp.mY = getPosition().y;
	temp.mZ = getPosition().z;

	return temp;
}

void GameObject::setMovementData(const PositionData& newState)
{
	setOrientation(newState.mOrientation);
	setRadius(newState.mRadius);
	setScale(newState.mScale);

	glm::quat newPosition;
	newPosition.w = newState.mW;
	newPosition.x = newState.mX;
	newPosition.y = newState.mY;
	newPosition.z = newState.mZ;

	setPosition(newPosition);
}

void GameObject::update(float deltaTime)
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
