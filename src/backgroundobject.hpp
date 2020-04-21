#pragma once
#include <string>
#include <iostream>
#include <tuple>

#include "sgct/log.h"
#include <glm/gtc/quaternion.hpp>

#include "glm/mat4x4.hpp"
#include "renderable.hpp"
#include "modelmanager.hpp"
#include "model.hpp"

class BackgroundObject : public Renderable {
public:
	//No default constructor
	BackgroundObject() = default;

	BackgroundObject(const std::string& objectModelName, float width, float height,
		const glm::quat& position);

	~BackgroundObject() {
		std::cout << "Deleting background";
	}

	void render(const glm::mat4& mvp) const {
		std::cout << "I'm rendering";
	}

	void drawBackground(const glm::mat4& mvp) const{
	  mQuad.render();
	}

private:
	Model mQuad;
};