#pragma once

#include "renderable.hpp"
#include "gameobject.hpp"
#include "model.hpp"

class SceneObject : public GameObject
{
public:
	//Ctor
	SceneObject(const unsigned objType, const glm::vec2 position, const float orientation, std::string modelName);

	//Render, overrides
	void render() const override;
private:
	Model& mModel;
	GLint transMatrixLoc = -1;
	GLint mvpMatrixLoc = -1;
};