#pragma once

#include "glm/mat4x4.hpp"

class Renderable {
public:
	virtual ~Renderable() { };
	virtual void render(const glm::mat4& mvp) const = 0;
};