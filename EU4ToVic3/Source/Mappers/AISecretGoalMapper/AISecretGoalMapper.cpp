#include "AISecretGoalMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <ranges>

namespace
{
std::vector<std::string> sortMap(const std::map<std::string, int>& theMap)
{
	std::vector<std::string> sorted;

	std::vector<std::pair<std::string, int>> pairs;
	for (const auto& theElement: theMap)
		pairs.emplace_back(theElement);

	std::sort(pairs.begin(), pairs.end(), [=](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
		return a.second > b.second;
	});

	for (const auto& tag: pairs | std::views::keys)
		sorted.emplace_back(tag);

	return sorted;
}
} // namespace


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

std::optional<std::string> mappers::AISecretGoalMapper::matchSecretGoal(const V3::Country& country,
	 const V3::Country& target,
	 const V3::ClayManager& clayManager) const
{
	std::map<std::string, int> goalValues;

	for (const auto& [goal, rules]: goalRules)
	{
		for (const auto& [value, rule]: rules)
			if (rule.matchGoal(country, target, clayManager))
				goalValues[goal] += value;
	}

	if (goalValues.empty())
		return std::nullopt;

	auto sorted = sortMap(goalValues);
	return *sorted.begin();
}
