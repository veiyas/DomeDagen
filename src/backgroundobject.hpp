#pragma once

#include <string>
#include <iostream>
#include <tuple>

#include "sgct/log.h"

#include "gameobject.hpp"
#include "geometryhandler.hpp"

struct ObjectData
{
public:
	//Position data
	float mRadius;
	float mOrientation;
};

class BackgroundObject : public GameObject, private GeometryHandler
{
public:
	//No default ctor
	BackgroundObject();

	//Dtor
	~BackgroundObject();

	//Only one background object
	BackgroundObject(const BackgroundObject&) = default;
	BackgroundObject& operator=(const BackgroundObject&) = delete;

	//Get/set data
	ObjectData getObjectData(bool isBackground) const;
	void setObjectData(const ObjectData& newState);

	//Render obejct
	void render(const glm::mat4& mvp) const override;

	//Activator + deactivator	
	void enableBackground() { mEnabled = true; }
	void disableBackground() { mEnabled = false; }

	//Mutators
	void setEnabled(bool state) { mEnabled = state; }
	void setSpeed(float speed) override {}; //Non-supported inherited function
	void setTurnSpeed(float turnSpeed) override {}; //Non-supported inherited function

private:

	//If disconnection of background is needed at start/end of game
	bool mEnabled = true;

	//Set shader data
	void setShaderData();
};