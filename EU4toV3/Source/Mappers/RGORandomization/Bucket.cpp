#include "Bucket.h"
#include "ParserHelpers.h"
#include "Log.h"

mappers::Bucket::Bucket(std::istream& theStream)
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString nameStr(theStream);
			name = nameStr.getString();
		});
	registerKeyword("climate", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString climateStr(theStream);
			if (climateStr.getString() == "any") 
			{
				wildClimate = true;
			}
			else 
			{
				climates.push_back(climateStr.getString());
			}
		});
	registerKeyword("terrain", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString terrainStr(theStream);
			if (terrainStr.getString() == "any")
			{
				wildTerrain = true;
			}
			else
			{
				terrains.push_back(terrainStr.getString());
			}
		});
	registerKeyword("fraction", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleDouble fractionDbl(theStream);
			fraction = fractionDbl.getDouble();
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}

bool mappers::Bucket::match(const std::string& provClimate, const std::string& provTerrain)
{
	auto climateMatch = wildClimate;
	if (!climateMatch)
	{
		for (const auto& climate : climates)
		{
			if (provClimate == climate)
			{
				climateMatch = true;
				break;
			}
		}
	}

	if (!climateMatch)
	{
		return false;
	}
	auto terrainMatch = wildTerrain;
	if (!terrainMatch)
	{
		for (const auto& terrain : terrains)
		{
			if (provTerrain == terrain)
			{
				terrainMatch = true;
				break;
			}
		}
	}

	return terrainMatch;
}

void mappers::Bucket::shuffle(std::default_random_engine& shuffler)
{
	std::shuffle(provinces.begin(), provinces.end(), shuffler);
	const auto numToShuffle = lround(fraction * provinces.size());
	if (numToShuffle < 2)
	{
		LOG(LogLevel::Debug) << "Skipping empty bucket " << name;
		return;
	}
	std::vector<std::string> rgos;
	rgos.reserve(numToShuffle);
	for (auto i = 0; i < numToShuffle; ++i)
	{
		rgos.push_back(provinces[i]->getRgoType());
	}
	std::shuffle(rgos.begin(), rgos.end(), shuffler);
	for (auto i = 0; i < numToShuffle; ++i)
	{
		provinces[i]->setRgoType(rgos[i]);
	}
	LOG(LogLevel::Debug) << "Shuffled " << numToShuffle << " provinces in bucket " << name;
}
