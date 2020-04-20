#pragma once

#include "gameobject.hpp"
#include "geometryhandler.hpp"

class Collectible : public GameObject, private GeometryHandler
{
public:	
	//Ctor used to load all collectibles into vector in Game class
	Collectible(const std::string objectModelName);

	//Copy ctor used to copy from central pool in Game into game
	Collectible(const Collectible&) = default;	

	//Inherited methods
	void render(const glm::mat4& mvp) const override;
	void update(float deltaTime) override {};
	void setSpeed(float speed) override {};

	//Set next node in list
	void setNext(Collectible* node);

	//Get next node
	Collectible* getNext();

	//Give collectiblepool access to privates
	friend class CollectiblePool;

private:
	bool mEnabled = false;
	float mSpeed;

	//Pointer used to implement free list functionality (constant time access!)
	Collectible* mNext = nullptr;

	constexpr bool isEnabled() const { return mEnabled; }
	void enable() { mEnabled = true; }
	void disable() { mEnabled = false; }
};

