#include "collectiblepool.hpp"
CollectiblePool* CollectiblePool::mInstance = nullptr;

CollectiblePool& CollectiblePool::instance()
{
	if (!mInstance)
	{
		mInstance = new CollectiblePool{};
	}
	return *mInstance;
}

void CollectiblePool::init()
{
	mInstance = new CollectiblePool{};
	sgct::Log::Info("Collectible object pool with %s elements created.",
					std::to_string(mInstance->mPool.size()).c_str());
}

CollectiblePool::CollectiblePool()
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
		Collectible tempCollectible{Collectible(trashModelNames[i % trashModelNames.size()])
		};
		mPool.push_back(std::move(tempCollectible));
	}

	//Set up pointer list
	mFirstAvailable = &mPool[0];
	for (size_t i = 0; i < mNumCollectibles - 1; i++)
	{
		mPool[i].setNext(&mPool[i + 1]);
	}
}



std::shared_ptr<GameObject> CollectiblePool::enableCollectible(const glm::vec3& pos)
{
	Collectible* newCollectible = mFirstAvailable;
	mFirstAvailable = newCollectible->getNext();

	std::shared_ptr<GameObject> temp{ newCollectible };
	temp->setPosition(pos);

	return temp;
}

void CollectiblePool::disableCollectible(Collectible& c)
{
	c.setNext(mFirstAvailable);
	mFirstAvailable = &c;
}
