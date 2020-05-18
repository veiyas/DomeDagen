#pragma once

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

struct PositionData
{
public:
	//TODO all of this data might not be needed to send
	float mRadius;
	float mOrientation;
	float mScale;

	//Quat stuff
	float mW;
	float mX;
	float mY;
	float mZ;
};

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
		COLLECTIBLE,
		BACKGROUND
	};
	GameObject() = delete;
	GameObject(const GameObject&) = default;
	GameObject(GameObject&&) = default;
	GameObject& operator=(const GameObject&) = default;
	GameObject& operator=(GameObject&& src) noexcept;

	//Ctor
	GameObject(const unsigned objType,
	           float radius,
	           const glm::quat& position,
	           float orientation,
	           const glm::quat& modelRotation = glm::quat(glm::vec3(glm::half_pi<float>(), 0.f, glm::pi<float>())));

	//Dtor implemented by subclasses
	virtual ~GameObject() override = default;

	////Render object, implemented by subclass
	//virtual void render(const glm::mat4& mvp, const glm::mat4& v) const = 0;

	//Update object (position, collision?)
	virtual void update(float deltaTime) = 0;
	//HACK This is very much a hack, and it would probably be better to keep different
	//types of gameobjects in different list to be able to use all functionality
	virtual void setSpeed(float speed) = 0;
	virtual void setTurnSpeed(float) {};

	//Calculates and returns the objects transformation matrix
	glm::mat4 getTransformation() const; // is there any reason for this not returning const&?

	//Accessors
	const float getScale() const { return mScale; }
	const float getRadius() const { return mRadius; }
	unsigned getObjType() const { return mObjType; }
	const glm::quat& getPosition() const { return mPosition; }
	const glm::quat& getModelRotation() const { return mModelRotation; }
	const float getOrientation() const { return mOrientation; }
	const PositionData getPositionData() const;	

	//Mutators
	void setRadius(float radius) { mRadius = radius; }
	void setScale(float scale) { mScale = scale; }
	void setPosition(const glm::quat position) { mPosition = position; }
	void setModelRotation(const glm::quat& modelRotation) { mModelRotation = modelRotation; }
	void setOrientation(float orientation) { mOrientation = orientation; }
	void setPositionData(const PositionData& newPosition);

private:
	//The position on the sphere represented by a unit quaternion
	glm::quat mPosition;

	//The radius of the sphere the object is positioned on
	float mRadius;

	//Rotation of model, i.e. rotation applied before all other transformations
	glm::quat mModelRotation;

	//The orientation in radians of the object (tangential to sphere)
	float mOrientation;

	//Scale of object, uniform
	float mScale;

	//Lightweight representation of object type
	unsigned const mObjType;
};