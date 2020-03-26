#pragma once

#include "renderable.hpp"
#include "model.hpp"

class SceneObject : public Renderable
{
public:
	SceneObject(Model& m);
	void render() const override;

private:
	Model mModel;
};