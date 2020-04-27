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
	mPool[mPool.size() - 1].setNext(nullptr);

	std::string sizeInfoString = std::to_string(mPool.size());
	sgct::Log::Info("Collectible pool with %s elements created", sizeInfoString.c_str());
}

void CollectiblePool::render(const glm::mat4& mvp) const
{
	for (size_t i = 0; i < mNumEnabled; i++)
	{
		mPool.at(i).render(mvp);
	}
}

void CollectiblePool::syncNewPoolState(const std::vector<CollectibleData>& newStates)
{	
	for (size_t i = 0; i < newStates.size(); i++)
	{
		mPool.at(i).setCollectibleData(newStates.at(i));
	}
	mNumEnabled = newStates.size();
}

void CollectiblePool::enableCollectible(const glm::vec3& pos)
{
	assert(mFirstAvailable != nullptr && "Object pool is full");

	Collectible& newCollectible = *mFirstAvailable; //VS shows a wrong warning here
	mFirstAvailable = newCollectible.getNext();

	newCollectible.setPosition(pos);
	newCollectible.enable();

	++mNumEnabled;
}

void CollectiblePool::disableCollectible(const size_t index)
{
	std::swap(mPool[index], mPool[mNumEnabled - 1]);

	auto& lastEnabledElement = mPool[index];
	auto& disabledElement = mPool[mNumEnabled - 1];

	//Thread enabled object
	lastEnabledElement.setNext(&mPool[index+1]);
	if (index > 0)
		mPool[index - 1].setNext(&lastEnabledElement);

	//Rethread and prime disabled object for usage
	disabledElement.disable();
	disabledElement.setNext(mFirstAvailable);
	if (mNumEnabled > 1)
		mPool[mNumEnabled - 2].setNext(&disabledElement);	
	mFirstAvailable = &disabledElement;

	--mNumEnabled;
}