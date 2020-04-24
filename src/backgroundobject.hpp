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
		//This texture binding probably only works if each mesh has 1 texture
		//glBindTexture(GL_TEXTURE_2D, mTextures[0].mId);

		////Dont know what happens if more than one texture per mesh, maybe GL_TEXTURE_2D_ARRAY should be used then (let's not)
		////for (const Texture& t : mTextures)
		////	glBindTexture(GL_TEXTURE_2D, t.mId);

		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
		//glBindVertexArray(0);
	  mQuad.render();
	}

private:
	Model mQuad;
};