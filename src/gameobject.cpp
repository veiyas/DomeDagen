#include "gameobject.hpp"
#include "game.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>

#include<iostream>

GameObject::GameObject(const std::string& objType, float radius, const glm::quat& position, float orientation)
	: mModel{ Game::getInstance().getModel(objType) }, mRadius{ radius }, mPosition{ position }, mOrientation{ orientation },
	mScale{ 1.f }, mObjType{ determineObjType(objType) }
{

}

void GameObject::update(float deltaTime)
{

}

glm::mat4 GameObject::getTranformation() const
{
	glm::mat4 trans  = glm::translate(glm::mat4(1.f), glm::vec3(0.f, mRadius, 0.f));
	glm::mat4 orient = glm::rotate(glm::mat4(1.f), mOrientation, glm::vec3(0, 1, 0));
	glm::mat4 rot    = glm::toMat4(mPosition);
	glm::mat4 scale  = glm::scale(glm::mat4(1.f), glm::vec3(mScale));

	//std::cout << glm::to_string(trans) << '\n';

	return rot * trans * orient * scale;
}

//TODO Rework with a list of object types and remove this ugly boilerplate
unsigned GameObject::determineObjType(const std::string& objType)
{
	if (objType == "player")
		return PLAYER;
	else if (objType == "fish")
		return SCENEOBJECT;
	else if (objType == "enemy")
		return ENEMY;
	else if (objType == "collectible")
		return COLLECTIBLE;
}