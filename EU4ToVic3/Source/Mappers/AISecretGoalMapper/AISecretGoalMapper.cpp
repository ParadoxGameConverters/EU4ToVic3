#include "AISecretGoalMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::AISecretGoalMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading AI Secret Goal mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << goalRules.size() << " goals loaded.";
}

void mappers::AISecretGoalMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& goalName, std::istream& theStream) {
		activeGoal = goalName;
		goalParser.parseStream(theStream);
	});

	goalParser.registerRegex(commonItems::integerRegex, [this](const std::string& value, std::istream& theStream) {
		if (!goalRules.contains(activeGoal))
			goalRules.emplace(activeGoal, std::vector<std::pair<int, AISecretGoalMapping>>{});
		goalRules.at(activeGoal).emplace_back(std::make_pair(std::stoi(value), AISecretGoalMapping(theStream)));
	});
	goalParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
