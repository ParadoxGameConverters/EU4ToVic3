#include "WarGoalMapper.h"
#include "ParserHelpers.h"
#include "Log.h"

mappers::WarGoalMapper::WarGoalMapper()
{
	LOG(LogLevel::Info) << "Parsing War Goals.";
	registerKeys();
	parseFile("configurables/war_goal_map.txt");
	clearRegisteredKeywords();
}

mappers::WarGoalMapper::WarGoalMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::WarGoalMapper::registerKeys()
{
	registerRegex("[a-z_]+", [this](const std::string& warGoal, std::istream& theStream)
		{
			const commonItems::stringList warList(theStream);
			for (const auto& eu4WarGoal : warList.getStrings()) warGoalMap[warGoal].insert(eu4WarGoal);
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);
}

std::optional<std::string> mappers::WarGoalMapper::translateWarGoal(const std::string& eu4WarGoal) const
{
	for (const auto& warGoalItr: warGoalMap) if (warGoalItr.second.count(eu4WarGoal)) return warGoalItr.first;
	return std::nullopt;
}
