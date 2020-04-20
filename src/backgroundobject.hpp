#pragma once
#include <string>
#include <iostream>
#include <tuple>

#include "sgct/log.h"
#include <glm/gtc/quaternion.hpp>

#include "glm/mat4x4.hpp"
#include "renderable.hpp"

class BackgroundObject : public Renderable {
public:
	//No default constructor
	BackgroundObject() = delete;

	BackgroundObject(const std::string& objectModelName, float width, float height,
		const glm::quat& position, const std::string& name);

	~BackgroundObject();

	void render(const glm::mat4& mvp) const;
};