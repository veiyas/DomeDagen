#include "gameobject.hpp"
#include "game.hpp"
#include "glm/gtc/matrix_transform.hpp"

GameObject::GameObject(const unsigned objType, const glm::vec2 position, const float orientation)
	: mObjType{ objType }, mPosition{ position }, mVelocity{ glm::vec2(0,0) }
	, mOrientation{orientation}, mScale{1.f}, mTransformation{glm::mat4{1.f}}
{
	mTransformation = glm::translate(mTransformation, glm::vec3(0.f, 18.f, -5.f));
}
void GameObject::update(float deltaTime)
{

}