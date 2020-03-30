#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <vector>
#include <string>
#include <glm/vec2.hpp>

#include "renderable.hpp"
#include "model.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

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
	GameObject(const std::string& objType, const glm::vec3 position, const float orientation);

	//Dtor implemented by subclasses
	virtual ~GameObject() override = default;

	//Render object, called by subclass to bind shaders
	virtual void render() const = 0;

	//Render model
	void renderModel() const { mModel.render(); };

	//Update object (position, collision?)
	virtual void update(float deltaTime);

	//Accessors
	const glm::vec2 getPosition() const { return mPosition; };
	const glm::vec2 getVelocity() const { return mVelocity; };
	const float getOrientation() const { return mOrientation; };
	const float getScale() const { return mScale; };
	unsigned getObjType() const { return mObjType; };
	glm::mat4 getTranformation() const { return mTransformation; };

	//Mutators
	void setPosition(const glm::vec3& position) { mPosition = position; };
	void setVelocity(const glm::vec2& velocity) { mVelocity = velocity; };
	void setOrientation(float orientation) { mOrientation = orientation; };
	void setScale(float scale) { mScale = scale; };
	void setTranformation(glm::vec3 transVec) { mTransformation = glm::translate(mTransformation, transVec); };

private:
	glm::vec3 mPosition;  // oklart om detta �r r�tt typ av position eg
	glm::vec2 mVelocity;  // eg borde ju speed r�cka i kombination med orientation
	float mOrientation;   // in radians
	float mScale;         // uniform in all directions
	unsigned const mObjType;

	//Reference to model to render
	Model& mModel;

	//Objects curren tranformation
	glm::mat4 mTransformation;

	//Determine and set mObjType, ugly but functional
	unsigned determineObjType(const std::string& objType);
};

#endif