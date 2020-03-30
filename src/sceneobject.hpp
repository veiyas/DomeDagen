#pragma once
#include <string>

#include "glm/gtc/type_ptr.hpp"

#include "geometryhandler.hpp"
#include "gameobject.hpp"


//For movable non-interactible objects, e.g fish in background
class SceneObject : public GameObject, private GeometryHandler
{
public:
	//Ctor
	SceneObject(const unsigned objectType, const std::string& objectModelName,
				const glm::vec3 position, const float orientation);

	//Render
	void render() const;

private:
};