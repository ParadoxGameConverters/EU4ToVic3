#include "BucketList.h"
#include "../../Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::BucketList::BucketList()
{
	// Initialise with the random seed from the EU4 save so that the shuffle
	// is deterministic for particular saves, but varies between saves.
	shuffler.seed(theConfiguration.getEU4RandomSeed());

	registerKeyword("bucket", [this](const std::string& key, std::istream& theStream) 
		{
			const Bucket newBucket(theStream);
			buckets.push_back(newBucket);
		}
	);
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseFile("configurables/rgo_randomization.txt");
	clearRegisteredKeywords();
}

void mappers::BucketList::putInBucket(std::shared_ptr<V2::Province> prov)
{
	const auto provClimate = prov->getClimate();
	if (provClimate.empty())
	{
		return;
	}
	const auto provTerrain = prov->getTerrain();
	if (provTerrain.empty() || provTerrain == "ocean")
	{
		return;
	}

	for (auto& bucket : buckets)
	{
		if (!bucket.match(provClimate, provTerrain))
		{
			continue;
		}
		bucket.addProvince(prov);
		break;
	}
}

void mappers::BucketList::shuffle()
{
	for (auto& bucket : buckets)
	{
		bucket.shuffle(shuffler);
	}
}

