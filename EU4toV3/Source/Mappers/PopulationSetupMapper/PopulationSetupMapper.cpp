#include "PopulationSetupMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PopulationSetupMapping.h"

void mappers::PopulationSetupMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing population setup rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Parsed " << literacySteps.size() << " literacy rules and " << wealthSteps.size() << " wealth rules.";
}

void mappers::PopulationSetupMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PopulationSetupMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const PopulationSetupMapping mapping(theStream);
		if (mapping.getLiteracy())
			literacySteps.emplace(*mapping.getLiteracy(), mapping.getEffect());
		if (mapping.getAverageDev())
			wealthSteps.emplace(*mapping.getAverageDev(), mapping.getEffect());
	});
}

std::string mappers::PopulationSetupMapper::getLiteracyEffectForLiteracy(const double literacy) const
{
	double delta = 999;
	std::string bestEffect;
	for (const auto& [literacyLevel, effect]: literacySteps)
	{
		const auto currentDelta = literacy - literacyLevel;
		if (currentDelta >= 0 && currentDelta < delta)
		{
			delta = currentDelta;
			bestEffect = effect;
		}
	}
	return bestEffect;
}

std::string mappers::PopulationSetupMapper::getWealthEffectForDev(const double dev) const
{
	double delta = 999;
	std::string bestEffect;
	for (const auto& [devLevel, effect]: wealthSteps)
	{
		const auto currentDelta = dev - devLevel;
		if (currentDelta >= 0 && currentDelta < delta)
		{
			delta = currentDelta;
			bestEffect = effect;
		}
	}
	return bestEffect;
}