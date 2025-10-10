#include "WesternizationMapper.h"
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "WesternizationMapping.h"

void mappers::WesternizationMapper::loadMappingRules(const std::filesystem::path& filePath)
{
	Log(LogLevel::Info) << "-> Parsing westernization groups.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Parsed " << westernizations.size() << " groups.";
}

void mappers::WesternizationMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::WesternizationMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const WesternizationMapping mapping(theStream);
		for (const auto& trait: mapping.getTraits())
		{
			westernizations.emplace(trait, mapping.getWesternization());
			literacies.emplace(trait, mapping.getLiteracy());
			industries.emplace(trait, mapping.getIndustry());
		}
	});
}

int mappers::WesternizationMapper::getWesternizationForCulture(const CultureDef& culture) const
{
	auto maxLevel = 0;
	bool foundCulture = false;
	for (const auto& tradition: culture.traditions)
	{
		if (westernizations.contains(tradition))
		{
			maxLevel = std::max(maxLevel, westernizations.at(tradition));
			foundCulture = true;
		}
	}
	if (westernizations.contains(culture.language))
	{
		maxLevel = std::max(maxLevel, westernizations.at(culture.language));
		foundCulture = true;
	}
	if (westernizations.contains(culture.heritage))
	{
		maxLevel = std::max(maxLevel, westernizations.at(culture.heritage));
		foundCulture = true;
	}
	if (!foundCulture)
	{
		Log(LogLevel::Warning) << "Culture " << culture.name << " has no westernization.txt traits linked!";
	}
	return maxLevel;
}

int mappers::WesternizationMapper::getIndustryForCulture(const CultureDef& culture) const
{
	auto maxLevel = 0;
	bool foundCulture = false;
	for (const auto& tradition: culture.traditions)
	{
		if (industries.contains(tradition))
		{
			maxLevel = std::max(maxLevel, industries.at(tradition));
			foundCulture = true;
		}
	}
	if (industries.contains(culture.language))
	{
		maxLevel = std::max(maxLevel, industries.at(culture.language));
		foundCulture = true;
	}
	if (industries.contains(culture.heritage))
	{
		maxLevel = std::max(maxLevel, industries.at(culture.heritage));
		foundCulture = true;
	}
	if (!foundCulture)
	{
		Log(LogLevel::Warning) << "Culture " << culture.name << " has no westernization.txt traits linked!";
	}
	return maxLevel;
}

int mappers::WesternizationMapper::getLiteracyForCulture(const CultureDef& culture) const
{
	auto maxLevel = 0;
	bool foundCulture = false;
	for (const auto& tradition: culture.traditions)
	{
		if (literacies.contains(tradition))
		{
			maxLevel = std::max(maxLevel, literacies.at(tradition));
			foundCulture = true;
		}
	}
	if (literacies.contains(culture.language))
	{
		maxLevel = std::max(maxLevel, literacies.at(culture.language));
		foundCulture = true;
	}
	if (literacies.contains(culture.heritage))
	{
		maxLevel = std::max(maxLevel, literacies.at(culture.heritage));
		foundCulture = true;
	}
	if (!foundCulture)
	{
		Log(LogLevel::Warning) << "Culture " << culture.name << " has no westernization.txt traits linked!";
	}
	return maxLevel;
}
