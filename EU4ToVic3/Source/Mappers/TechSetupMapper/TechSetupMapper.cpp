#include "TechSetupMapper.h"
#include "CommonRegexes.h"
#include "Log.h"

void mappers::TechSetupMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Tech Setup Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << trackTechScores.size() << " tracks loaded.";
}

void mappers::TechSetupMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TechSetupMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& trackName, std::istream& theStream) {
		trackTechScores.emplace(trackName, TechSetupMapping(theStream));
	});
}

std::set<std::string> mappers::TechSetupMapper::getTechsForScoreTrack(const std::string& track, const double score) const
{
	if (!trackTechScores.contains(track))
	{
		Log(LogLevel::Warning) << "Attempting to get techs from track: " << track << " which doesn't exist in tech_setup.txt!";
		return {};
	}

	const auto& techMap = trackTechScores.at(track).getTechScores();
	std::set<std::string> techs;
	for (const auto& [tech, techScore]: techMap)
		if (score >= techScore)
			techs.emplace(tech);
	return techs;
}
