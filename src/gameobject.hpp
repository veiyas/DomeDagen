#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "renderable.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include <vector>
#include <string>
#include <glm/vec2.hpp>

//Enumerator to keep track of object type
enum objectType{
	PLAYER,
	ENEMY,
	COLLECTIBLE
};

class GameObject : public Renderable {
public:
	//No default ctor
	GameObject() = delete;

	//Ctor
	GameObject(const unsigned objType, const glm::vec2 position, const float orientation);

	//Dtor implemented by subclasses
	virtual ~GameObject() override = default;

	//Render implemented by subclasses
	virtual void render() const = 0;

	//Update object (position, collision?)
	virtual void update(float delta_time);

	//Accessors
	const glm::vec2 getPosition() const { return mPosition; };
	const glm::vec2 getVelocity() const { return mVelocity; };
	const float getOrientation() const { return mOrientation; };
	const float getScale() const { return mScale; };
	unsigned getObjType() const { return mObjType; };

	//Mutators
	void setPosition(const glm::vec2& position) { mPosition = position; };
	void setVelocity(const glm::vec2& velocity) { mVelocity = velocity; };
	void setOrientation(const float& orientation) { mOrientation = orientation; };
	void setScale(const float& scale) { mScale = scale; };

private:
	glm::vec2 mPosition;  // oklart om detta �r r�tt typ av position eg
	glm::vec2 mVelocity;  // eg borde ju speed r�cka i kombination med orientation
	float mOrientation;   // in radians
	float mScale;         // uniform in all directions
	unsigned const mObjType;
};

#endif