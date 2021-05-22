#include "StartingTechMapper.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::StartingTechMapper::StartingTechMapper()
{
	LOG(LogLevel::Info) << "Parsing starting techs.";
	registerKeyword("tech", [this](const std::string& techGroup, std::istream& theStream)
		{
			const StartingTech startingTech(theStream);
			startingTechs.emplace_back(startingTech);
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseFile("configurables/starting_technologies.txt");
	clearRegisteredKeywords();
}

std::vector<std::string> mappers::StartingTechMapper::getTechsForGroupAndScore(const std::string& group, const double score) const
{
	std::vector<std::string> toReturn;
	for (const auto& tech : startingTechs) if (tech.getGroup() == group && score >= tech.getScore()) toReturn.emplace_back(tech.getName());
	return toReturn;
}
