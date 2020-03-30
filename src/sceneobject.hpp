#pragma once

#include "renderable.hpp"
#include "gameobject.hpp"
#include "model.hpp"

class SceneObject : public GameObject
{
public:
	//Ctor
	SceneObject(const std::string& objType, const glm::vec3 position, const float orientation);

	//Render
	void render() const;

private:
	//Shader matrix locations
	GLint mTransMatrixLoc = -1;
	GLint mMvpMatrixLoc = -1;

	const std::string mModelName;
};