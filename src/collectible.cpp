#include "collectible.hpp"

Collectible::Collectible()
	:GameObject{ GameObject::COLLECTIBLE, 50.f, glm::vec3(1.f, 0.f, 0.f), 0.f }
	,GeometryHandler{ "collectible", "can1" }
	,mEnabled{ false }, mNext{ nullptr }
{
}

Collectible::Collectible(const std::string objectModelName)
	:GameObject{ GameObject::COLLECTIBLE, 50.f, glm::vec3(1.f, 0.f, 0.f), 0.f }
	,GeometryHandler{ "collectible", objectModelName }
	,mEnabled{false}, mNext{nullptr}
{
	mMvpMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "mvp");
	mTransMatrixLoc = glGetUniformLocation(mShaderProgram.id(), "transformation");
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

void Collectible::render(const glm::mat4& mvp) const
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

CollectibleData Collectible::getCollectibleData(unsigned index) const
{
	CollectibleData temp;

	//Positional data
	temp.mPosData.mOrientation = getOrientation();
	temp.mPosData.mRadius = getRadius();
	temp.mPosData.mScale = getScale();
	temp.mPosData.mSpeed = 0.1f;

	//Quat stuff
	temp.mPosData.mW = getPosition().w;
	temp.mPosData.mX = getPosition().x;
	temp.mPosData.mY = getPosition().y;
	temp.mPosData.mZ = getPosition().z;

	temp.mIndex = index;

	return temp;
}

void Collectible::setCollectibleData(const CollectibleData& newState)
{
	//Position data
	setOrientation(newState.mPosData.mOrientation);
	setRadius(newState.mPosData.mRadius);
	setScale(newState.mPosData.mScale);

	//Quat stuff
	glm::quat newPosition;
	newPosition.w = newState.mPosData.mW;
	newPosition.x = newState.mPosData.mX;
	newPosition.y = newState.mPosData.mY;
	newPosition.z = newState.mPosData.mZ;
	setPosition(newPosition);

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
