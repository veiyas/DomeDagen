#pragma once

#include <vector>

#include "sgct/log.h"

#include "collectible.hpp"
#include "constants.hpp"

//Contain all collectibles with object pool design pattern
//The pool will be stable partitioned with regard to enabled objects
//Explicit singleton because of include feedback loops to use in main.cpp
class CollectiblePool
{
public:
	//Singleton stuff
	static CollectiblePool& instance();
	static void init();

	//Returns shared_ptr and points mFirstAvailable to next in list. O(1)!
	std::shared_ptr<GameObject> enableCollectible(const glm::vec3& pos);

	//Deactivate and return the collectible to available list
	void disableCollectible(Collectible& c);

private:
	//Singleton instance
	static CollectiblePool* mInstance;

	//Ctor creates pool of mNumCollectibles collectibles
	//Alternates between all trash models
	//Points mFirstAvailable to first element	
	CollectiblePool();

	//The pool of collectible objects
	std::vector<Collectible> mPool;

	//Pointer to first available object ready to import into the game
	Collectible* mFirstAvailable;

	//Limit on number of objects in pool
	static constexpr unsigned mNumCollectibles = 200;
	
};