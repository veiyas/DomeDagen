#include "gameobject.hpp"

GameObject::GameObject(const unsigned objType, const glm::vec3 position, const float orientation)
	: mPosition{ position }, mVelocity{ glm::vec2(0,0) }
	, mOrientation{orientation}, mScale{1.f}, mObjType{ objType }
{

}

void GameObject::update(float deltaTime)
{

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