#include "collectiblepool.hpp"

CollectiblePool::CollectiblePool()
{
	auto trashModelNames = allModelNames;
	auto fishPtr = std::find(allModelNames.begin(), allModelNames.end(), "fish");
	auto diverPtr = std::find(allModelNames.begin(), allModelNames.end(), "diver");
	
	for (size_t i = 0; i < mNumCollectibles; i++)
	{
		mPool.push_back(Collectible{ trashModelNames[i % trashModelNames.size()] });
	}
}
