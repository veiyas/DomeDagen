#include "collectiblepool.hpp"

void CollectiblePool::init()
{
	ZoneScoped;
	//Copy trash models from allModelNames
	std::vector<std::string> trashModelNames;
	for (const std::string& name : allModelNames)
	{
		if (name == "fish" || name == "diver" || name == "background")
			continue;

		trashModelNames.push_back(name);
	}

	//Add objects to pool, set up list
	for (size_t i = 0; i < mMAXNUMCOLLECTIBLES; i++)
	{
		Collectible tempCollectible{ Collectible(trashModelNames[i % trashModelNames.size()])
		};
		mPool.push_back(std::move(tempCollectible));
	}

	//Set up pointer list
	mFirstAvailable = &mPool[0];
	for (size_t i = 0; i < mMAXNUMCOLLECTIBLES - 1; i++)
	{
		mPool[i].setNext(&mPool[i + 1]);
	}
	mPool[mPool.size() - 1].setNext(nullptr);

	std::string sizeInfoString = std::to_string(mPool.size());
	sgct::Log::Info("Collectible pool with %s elements created", sizeInfoString.c_str());
}

void CollectiblePool::render(const glm::mat4& mvp, const glm::mat4& v) const
{
	ZoneScoped;
	if (mPool.size() > 0)
	{
		auto const& collectibleShader = sgct::ShaderManager::instance().shaderProgram("collectible");
		collectibleShader.bind();
		for (size_t i = 0; i < mNumEnabled; i++)
		{
			//if (!mPool[i].isEnabled())
			//	continue;
			mPool[i].render(mvp, v);
		}
		collectibleShader.unbind();
	}
}

std::vector<CollectibleData> CollectiblePool::getPoolState() const
{
	ZoneScoped;
	std::vector<CollectibleData> poolData;
	poolData.reserve(mNumEnabled);

	for (size_t i = 0; i < mPool.size(); i++)
	{
		if (mPool[i].isEnabled())
		{
			poolData.push_back(mPool[i].getCollectibleData(i));
		}
	}

	return poolData;
}

void CollectiblePool::enableCollectible(const glm::vec3& pos)
{
	ZoneScoped;
	if (mFirstAvailable == nullptr)
		return;

	Collectible& newCollectible = *mFirstAvailable; //VS shows a wrong warning here
	mFirstAvailable = newCollectible.getNext();

	newCollectible.setPosition(pos);
	newCollectible.enable();

	++mNumEnabled;
}

void CollectiblePool::disableCollectible(const size_t index)
{
	ZoneScoped;
	/*
	
		This swap code is hella good for performance
		Unfortunately it doesn't play well with node sync

	*/
	//std::swap(mPool[index], mPool[mNumEnabled - 1]);

	//auto& lastEnabledElement = mPool[index];
	//auto& disabledElement = mPool[mNumEnabled - 1];

	////Thread enabled object
	//lastEnabledElement.setNext(&mPool[index+1]);
	//if (index > 0)
	//	mPool[index - 1].setNext(&lastEnabledElement);

	////Rethread and prime disabled object for usage
	//disabledElement.disable();
	//disabledElement.setNext(mFirstAvailable);
	//if (mNumEnabled > 1)
	//	mPool[mNumEnabled - 2].setNext(&disabledElement);	
	//mFirstAvailable = &disabledElement;

	auto& disabledElement = mPool[index];
	disabledElement.disable();
	disabledElement.setNext(mFirstAvailable);
	mFirstAvailable = &disabledElement;

	--mNumEnabled;
}