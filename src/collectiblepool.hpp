#include <vector>

#include "collectible.hpp"

//Contain all collectibles with object pool design pattern
//The pool will be stable partitioned with regard to enabled objects
class CollectiblePool
{
public:
	//Ctor creates pool of mNumCollectibles collectibles
	//Alternates between all trash models
	CollectiblePool();
private:
	size_t mEnabledLimit = 0;
	std::vector<Collectible> mPool;

	static constexpr unsigned mNumCollectibles = 200;
	
};