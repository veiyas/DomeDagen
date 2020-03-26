#include <string>

#include "sceneobject.hpp"
#include "game.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include "glm/gtc/type_ptr.hpp"

SceneObject::SceneObject(const unsigned objType, const glm::vec2 position, const float orientation, std::string modelName)
	: GameObject{ objType, position, orientation }, mModel{Game::getInstance().getModel(modelName)}
{
	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("sceneobject");
	prg.bind();
	mvpMatrixLoc = glGetUniformLocation(prg.id(), "mvp");
	transMatrixLoc = glGetUniformLocation(prg.id(), "transformation");
	prg.unbind();
}

void SceneObject::render() const
{
	const sgct::ShaderProgram& prg = sgct::ShaderManager::instance().shaderProgram("sceneobject");
	prg.bind();

	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(Game::getInstance().getMVP()));
	glUniformMatrix4fv(transMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTranformation()));
	mModel.render();

	prg.unbind();
}
