#include "collectiblepool.hpp"

void CollectiblePool::init()
{
	//Copy trash models from allModelNames
	std::vector<std::string> trashModelNames;
	for (const std::string& name : allModelNames)
	{
		if (name == "fish")
			continue;
		else if (name == "diver")
			continue;

		trashModelNames.push_back(name);
	}

	//Add objects to pool, set up list
	for (size_t i = 0; i < mNumCollectibles; i++)
	{
		Collectible tempCollectible{ Collectible(trashModelNames[i % trashModelNames.size()])
		};
		mPool.push_back(std::move(tempCollectible));
	}

	//Set up pointer list
	mFirstAvailable = &mPool[0];
	for (size_t i = 0; i < mNumCollectibles - 1; i++)
	{
		mPool[i].setNext(&mPool[i + 1]);
	}

	std::string sizeInfoString = std::to_string(mPool.size());
	sgct::Log::Info("Collectible pool with %s elements created", sizeInfoString.c_str());
}

Collectible CollectiblePool::enableCollectible(const glm::vec3& pos)
{
	Collectible* newCollectible = mFirstAvailable;
	mFirstAvailable = newCollectible->getNext();

	newCollectible->setPosition(pos);

	++mNumEnabled;

	return *newCollectible;
}

void CollectiblePool::disableCollectible(Collectible& c)
{
	c.setNext(mFirstAvailable);
	mFirstAvailable = &c;
	--mNumEnabled;
}
