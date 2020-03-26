#include "sceneobject.hpp"

SceneObject::SceneObject(Model& m)
{
	mModel = m;
}

void SceneObject::render() const
{
	mModel.render();
}
