#include "WesternizationMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "WesternizationMapping.h"

void mappers::WesternizationMapper::loadRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "Parsing technology groups.";
	registerKeyword("link", [this](std::istream& theStream) {
		const WesternizationMapping mapping(theStream);
		for (const auto& trait: mapping.getTraits())
		{
			westernizations.emplace(trait, mapping.getWesternization());
			literacies.emplace(trait, mapping.getLiteracy());
			industries.emplace(trait, mapping.getIndustry());
		}
	});

	parseFile(filePath);
	clearRegisteredKeywords();
}

int mappers::WesternizationMapper::getWesternizationForTraits(const std::set<std::string>& traits) const
{
	auto maxLevel = 0;
	for (const auto& trait: traits)
		if (westernizations.contains(trait))
		{
			if (westernizations.at(trait) > maxLevel)
				maxLevel = westernizations.at(trait);
		}
		else
		{
			Log(LogLevel::Warning) << "Trait " << trait << " has no westernization.txt link!";
		}
	return maxLevel;
}

int mappers::WesternizationMapper::getIndustryForTraits(const std::set<std::string>& traits) const
{
	auto maxLevel = 0;
	for (const auto& trait: traits)
		if (industries.contains(trait))
		{
			if (industries.at(trait) > maxLevel)
				maxLevel = industries.at(trait);
		}
		else
		{
			Log(LogLevel::Warning) << "Trait " << trait << " has no westernization.txt link!";
		}
	return maxLevel;
}

int mappers::WesternizationMapper::getLiteracyForTraits(const std::set<std::string>& traits) const
{
	auto maxLevel = 0;
	for (const auto& trait: traits)
		if (literacies.contains(trait))
		{
			if (literacies.at(trait) > maxLevel)
				maxLevel = literacies.at(trait);
		}
		else
		{
			Log(LogLevel::Warning) << "Trait " << trait << " has no westernization.txt link!";
		}
	return maxLevel;
}
