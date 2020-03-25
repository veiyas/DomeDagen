#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "renderable.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include <vector>
#include <string>
#include <glm/vec2.hpp>

class GameObject : public Renderable {
public:
	GameObject() = default;
	virtual ~GameObject() { };  // hur �r det med virtuella destructors nu igen

	void render() const override;
	virtual void update(float deltaTime);

	// accessors
	const glm::vec2& getPosition() const { return mPosition; };
	const glm::vec2& getVelocity() const { return mVelocity; };
	float getOrientation() const { return mOrientation; };
	float getScale() const { return mScale; };

	// mutators
	void setPosition(const glm::vec2& position) { mPosition = position; };
	void setVelocity(const glm::vec2& velocity) { mVelocity = velocity; };
	void setOrientation(float orientation) { mOrientation = orientation; };
	void setScale(float scale) { mScale = scale; };

private:
	glm::vec2 mPosition;  // oklart om detta �r r�tt typ av position eg
	glm::vec2 mVelocity;  // eg borde ju speed r�cka i kombination med orientation
	float mOrientation;   // in radians
	float mScale;         // uniform in all directions
};

#endif