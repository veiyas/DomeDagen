#include <string>

#include "sceneobject.hpp"
#include "game.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glm/gtc/type_ptr.hpp"

SceneObject::SceneObject(const std::string& objType, float radius, const glm::quat& position, const float orientation)
	: GameObject{ objType, radius, position, orientation }
{
	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("sceneobject");
	prg.bind();
	mMvpMatrixLoc = glGetUniformLocation(prg.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(prg.id(), "transformation");
	prg.unbind();
}

void SceneObject::render() const
{
	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("sceneobject");
	prg.bind();

	glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTranformation()));
	this->renderModel();

	prg.unbind();
}