#pragma once

#include <vector>

#include "sgct/log.h"

#include "collectible.hpp"
#include "constants.hpp"

//Contain all collectibles with object pool design pattern
//Game contains an instance of this class
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
	void render(const glm::mat4& mvp) const;

	//Returns shared_ptr and points mFirstAvailable to next in list. O(1)!
	Collectible& enableCollectible(const glm::vec3& pos);

	//Deactivate and return the collectible to available list
	void disableCollectible(const size_t index);

	//Accessor
	size_t getNumEnabled() const { return mNumEnabled; }

	//The pool of collectible objects
	std::vector<Collectible> mPool;

private:
	//Number of enabled objects
	size_t mNumEnabled = 0;

	//Pointer to first available object ready to import into the game
	Collectible* mFirstAvailable = nullptr;

	//Limit on number of objects in pool
	static constexpr unsigned mNumCollectibles = 300;
	
};