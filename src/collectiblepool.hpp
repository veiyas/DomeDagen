#pragma once

#include <vector>

#include "sgct/log.h"
#include "sgct/shadermanager.h"
#include "sgct/profiling.h"

#include "collectible.hpp"
#include "constants.hpp"

//Contain all collectibles with object pool design pattern
//Game contains an instance of this class
//Pool is stable partitioned with enabled objects in front
class CollectiblePool
{
public:
	//Ctor creates pool of mNumCollectibles collectibles
	//Alternates between all trash models
	//Points mFirstAvailable to first element	
	CollectiblePool() = default;

	//creates pool of mNumCollectibles collectibles
	//Alternates between all trash models
	//Points mFirstAvailable to first element	
	void init();

	//Render enabled objects
	void render(const glm::mat4& mvp, const glm::mat4& v) const;
	
	//Get collectiblepool state
	std::vector<CollectibleData> getPoolState();

	//Returns shared_ptr and points mFirstAvailable to next in list. O(1)!
	void enableCollectible(const glm::vec3& pos);

	//Deactivates object at index and swaps to front (Used on master)
	void disableCollectibleAndSwap(const size_t index);

	//Simply deactivates element at index
	void disableCollectible(const size_t index) { mPool[index].disable(); }

	//Operator overloading to hide internal data
	Collectible& operator[](const size_t i) { return mPool[i]; }

	//Accessors/Mutator
	size_t getNumEnabled() const { return mNumEnabled; }
	void setNumEnabled(size_t size) { mNumEnabled = size; }

	//Max number of collectibles
	static constexpr unsigned mMAXNUMCOLLECTIBLES = 300;

private:
	//The pool of collectible objects
	std::vector<Collectible> mPool;

	//Number of enabled objects
	size_t mNumEnabled = 0;

	//Pointer to first available object ready to import into the game
	Collectible* mFirstAvailable = nullptr;

	//Limit on number of objects in pool
	
	
};