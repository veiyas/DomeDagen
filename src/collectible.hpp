#pragma once

#include "gameobject.hpp"
#include "geometryhandler.hpp"

struct CollectibleData
{
	unsigned int mIndex;
};

class Collectible : public GameObject, private GeometryHandler
{
public:	
	Collectible();
	//Ctor used to load all collectibles into vector in Game class
	Collectible(const std::string objectModelName);

	//WARNING: these ctors might be leaky
	//TODO These are not used anymore, remove?
	Collectible(const Collectible& src);
	Collectible(Collectible&& src) noexcept;
	Collectible& operator=(const Collectible& src);
	Collectible& operator=(Collectible&& src) noexcept;
	~Collectible() override = default;

	//Inherited methods
	void render(const glm::mat4& mvp) const override;
	void update(float deltaTime) override {};
	void setSpeed(float speed) override {};

	//Sync methods
	CollectibleData getCollectibleData(unsigned index) const;
	void setCollectibleData(const PositionData& newPosData);

	//Set next node in list
	void setNext(Collectible* node);

	//Get next node
	Collectible* getNext();

	const bool isEnabled() const { return mEnabled; }

	//Give collectiblepool access to privates
	friend class CollectiblePool;

private:
	//Is this collectible active in the game?
	bool mEnabled;

	static constexpr float mSpeed = 0.1f;

	//Pointer to implement free list functionality (constant time access!)
	Collectible* mNext;
	
	void enable() { mEnabled = true; }
	void disable() { mEnabled = false; }
};

