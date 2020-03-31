#include "gameobject.hpp"

#include<glm/common.hpp>
#include<glm/gtx/string_cast.hpp>

#include<iostream>

GameObject::GameObject(const unsigned objType, float radius, const glm::quat& position, float orientation)
	: mRadius{ radius }, mPosition{ position }, mOrientation{ orientation },
	 mScale{ 1.f }, mObjType{ objType }
{

}

void GameObject::update(float deltaTime)
{

}

glm::mat4 GameObject::getTransformation() const
{
	glm::mat4 trans  = glm::translate(glm::mat4(1.f), glm::vec3(0.f, mRadius, 0.f));
	glm::mat4 orient = glm::rotate(glm::mat4(1.f), mOrientation, glm::vec3(0, 1, 0));
	glm::mat4 rot    = glm::toMat4(mPosition);
	glm::mat4 scale  = glm::scale(glm::mat4(1.f), glm::vec3(mScale));

	//std::cout << glm::to_string(trans) << '\n';

	return rot * trans * orient * scale;
}