#include "collectible.hpp"

Collectible::Collectible()
	:Collectible{"can1"}
{
	setShaderData();
}

Collectible::Collectible(const std::string objectModelName)
	:GameObject{ GameObject::COLLECTIBLE, 50.f, glm::vec3(1.f, 0.f, 0.f), 0.f }
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
	if (mEnabled)
	{
		mShaderProgram.bind();

		glUniformMatrix4fv(mMvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(mTransMatrixLoc, 1, GL_FALSE, glm::value_ptr(getTransformation()));
		this->renderModel();

		mShaderProgram.unbind();
	}
}

void Collectible::update(float deltaTime)
{
	setModelRotation(getModelRotation() * glm::quat(deltaTime * 0.1f * glm::vec3(1.f, 1.f, 1.f)));
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
