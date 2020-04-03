#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glad/glad.h"

#include "renderable.hpp"
#include "model.hpp"


//A GameObject is located att the surface of a sphere
//and it has a side that is always facing origin.
class GameObject : public Renderable
{
public:
	//Enumerator to keep track of object type
	enum objectType {
		PLAYER,
		SCENEOBJECT,
		ENEMY,
		COLLECTIBLE
	};

	//No default ctor
	GameObject() = delete;

	//Ctor
	GameObject(const unsigned objType, float radius, const glm::quat& position, float orientation);

	//Dtor implemented by subclasses
	virtual ~GameObject() override = default;

	//Render object, called by subclass to bind shaders
	virtual void render() const = 0;

	//Update object (position, collision?)
	virtual void update(float deltaTime);

	//Calculates and returns the objects transformation matrix
	glm::mat4 getTransformation() const; // is there any reason for this not returning const&?

	//Accessors
	const float getScale() const { return mScale; }
	unsigned getObjType() const { return mObjType; }
	const glm::quat& getPosition() const { return mPosition; }
	const float getOrientation() const { return mOrientation; }	

	//Mutators
	void setScale(float scale) { mScale = scale; }
	void setPosition(const glm::quat position) { mPosition = position; }
	void setOrientation(float orientation) { mOrientation = orientation; } //overflow?

private:
	//The position on the sphere represented by a unit quaternion
	glm::quat mPosition;

	//The radius of the sphere the object is positioned on
	float mRadius;

	//The orientation in radians of the object (tangential to sphere)
	float mOrientation;

	//Scale of object, uniform
	float mScale;

	//Lightweight representation of object type
	unsigned const mObjType;
};

#endif