#include "collectible.hpp"
#include "constants.hpp"

Collectible::Collectible()
	:Collectible{"can1"}
{
	setShaderData();
}

Collectible::Collectible(const std::string objectModelName)
	:GameObject{ GameObject::COLLECTIBLE, DOMERADIUS, glm::vec3(1.f, 0.f, 0.f), 0.f, COLLECTIBLESCALE }
	,GeometryHandler{ "collectible", objectModelName }
	,mEnabled{false}, mNext{nullptr}
{
	setShaderData();
}

Collectible::Collectible(const Collectible& src)
	:GameObject{ src }, GeometryHandler{ src }
{
	mNext = src.mNext;
	mEnabled = src.mEnabled;
}

Collectible::Collectible(Collectible&& src) noexcept
	:GameObject{ std::move(src) }, GeometryHandler{ std::move(src) }
{
	mNext = src.mNext;
	mEnabled = src.mEnabled;
}

Collectible& Collectible::operator=(const Collectible& src)
{
	auto copy = src;
	std::swap(*this, copy);

	return *this;
}

Collectible& Collectible::operator=(Collectible&& src) noexcept
{
	GameObject::operator= (std::move(src));
	GeometryHandler::operator= (std::move(src));
	std::swap(mEnabled, src.mEnabled);
	std::swap(mNext, src.mNext);

	return *this;
}

void Collectible::render(const glm::mat4& mvp, const glm::mat4& v) const
{
	//ZoneScoped;
	if (mEnabled)
	{
		mShaderProgram.bind();

		glm::vec3 cameraPos = glm::vec3((inverse(v))[3]);
		glUniform3fv(mCameraPosLoc, 1, glm::value_ptr(cameraPos));

		glm::mat4 transformation = getTransformation();
		glm::mat3 normalMatrix(glm::transpose(glm::inverse(transformation)));

		glUniformMatrix3fv(mNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(mViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		this->renderModel();

		mShaderProgram.unbind();
	}
}

void Collectible::update(float deltaTime)
{
	setModelRotation(getModelRotation() * glm::quat(deltaTime * 1.2f * glm::vec3(1.f, 1.f, 1.f)));
}

CollectibleData Collectible::getCollectibleData(unsigned index) const
{
	CollectibleData temp;

	temp.mIndex = index;

	return temp;
}

void Collectible::setCollectibleData(const PositionData& newPosData)
{
	setPositionData(newPosData);

	enable();
}

void Collectible::setNext(Collectible* node)
{
	mNext = node;
}

Collectible* Collectible::getNext()
{
	return mNext;
}
