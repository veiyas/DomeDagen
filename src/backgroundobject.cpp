#include "backgroundobject.hpp"

BackgroundObject::BackgroundObject(const std::string& objectModelName, float width, float height,
	const glm::quat& position) {
	mQuad = ModelManager::instance().getModel(objectModelName);
	std::cout << "Adding background object";
}