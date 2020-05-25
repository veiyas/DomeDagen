#pragma once

#include "gameobject.hpp"
#include "geometryhandler.hpp"

struct CollectibleData
{
	int mModelIndex;
};

class Collectible : public GameObject, private GeometryHandler
{
public:
	//Give collectiblepool access to privates
	friend class CollectiblePool;
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
	void render(const glm::mat4& mvp, const glm::mat4& v) const override;
	void update(float deltaTime) override;
	void setSpeed(float speed) override {};

	//Sync methods
	CollectibleData getCollectibleData(unsigned index);
	void setCollectibleData(const PositionData& newPosData, const int modelIndex);

	//Set next node in list
	void setNext(Collectible* node);

	//Get next node
	Collectible* getNext() const;

	const bool isEnabled() const { return mEnabled; }
private:
	//Is this collectible active in the game?
	bool mEnabled;

	static constexpr float mSpeed = 0.1f;

	//Pointer to implement free list functionality (constant time access!)
	Collectible* mNext;
	
	void enable() { mEnabled = true; }
	void disable() { mEnabled = false; }
};

