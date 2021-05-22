#include "TechGroupsMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "TechGroups.h"
#include "CommonRegexes.h"

mappers::TechGroupsMapper::TechGroupsMapper()
{
	LOG(LogLevel::Info) << "Parsing technology groups.";
	registerRegex(commonItems::catchallRegex, [this](const std::string& techGroup, std::istream& theStream) {
		const TechGroups techGroups(theStream);

		westernizations[techGroup] = techGroups.getWesternization();
		literacies[techGroup] = techGroups.getLiteracyBoost();
		for (const auto& culture: techGroups.getCultures())
		{
			const auto& culWestItr = cultureWesternization.find(culture);
			if (culWestItr != cultureWesternization.end())
				Log(LogLevel::Warning) << "Culture: " << culture << " already has a westernization score of " << culWestItr->second << "! Overriding with " << techGroups.getWesternization() << ".";
			cultureWesternization[culture] = techGroups.getWesternization();
		}
	});

	parseFile("configurables/tech_groups.txt");
	clearRegisteredKeywords();
}

int mappers::TechGroupsMapper::getWesternizationFromTechGroup(const std::string& techGroupName) const
{
	const auto& techGroup = westernizations.find(techGroupName);
	if (techGroup != westernizations.end())
		return techGroup->second;
	LOG(LogLevel::Warning) << "Unknown tech group: " << techGroupName;
	return 0;
}

int mappers::TechGroupsMapper::getWesternizationFromCulture(const std::string& culture) const
{
	const auto& westItr = cultureWesternization.find(culture);
	if (westItr != cultureWesternization.end())
		return westItr->second;
	LOG(LogLevel::Warning) << "No westernization for culture: " << culture;
	return 0;
}

int mappers::TechGroupsMapper::getLiteracyFromTechGroup(const std::string& techGroupName) const
{
	const auto& techGroup = literacies.find(techGroupName);
	if (techGroup != literacies.end())
		return techGroup->second;
	LOG(LogLevel::Warning) << "Unknown tech group: " << techGroupName;
	return 0;
}
