#include "gameobject.hpp"

GameObject::GameObject(const unsigned objType, const glm::vec2 position, const float orientation)
	: mObjType{ objType }, mPosition{ position }, mVelocity{ glm::vec2(0,0) }, mOrientation{orientation}, mScale{1.f}
{

}

void GameObject::update(float deltaTime)
{

}
