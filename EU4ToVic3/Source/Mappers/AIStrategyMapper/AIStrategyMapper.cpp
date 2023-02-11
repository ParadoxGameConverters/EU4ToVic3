#include "AIStrategyMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::AIStrategyMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading AI Strategy mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << admStrategies.size() + dipStrategies.size() + polStrategies.size() << " strategies loaded.";
}

void mappers::AIStrategyMapper::registerKeys()
{
	registerKeyword("adm", [this](std::istream& theStream) {
		admParser.parseStream(theStream);
	});
	registerKeyword("dip", [this](std::istream& theStream) {
		dipParser.parseStream(theStream);
	});
	registerKeyword("pol", [this](std::istream& theStream) {
		polParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	admParser.registerKeyword("default", [this](std::istream& theStream) {
		defaultAdmStrategy = commonItems::getString(theStream);
	});
	admParser.registerRegex(commonItems::catchallRegex, [this](const std::string& strategyName, std::istream& theStream) {
		admStrategies.emplace(strategyName, AIStrategyMapping(theStream));
	});

	dipParser.registerKeyword("default", [this](std::istream& theStream) {
		defaultDipStrategy = commonItems::getString(theStream);
	});
	dipParser.registerRegex(commonItems::catchallRegex, [this](const std::string& strategyName, std::istream& theStream) {
		dipStrategies.emplace(strategyName, AIStrategyMapping(theStream));
	});

	polParser.registerKeyword("default", [this](std::istream& theStream) {
		defaultPolStrategy = commonItems::getString(theStream);
	});
	polParser.registerRegex(commonItems::catchallRegex, [this](const std::string& strategyName, std::istream& theStream) {
		polStrategies.emplace(strategyName, AIStrategyMapping(theStream));
	});
}

std::map<std::string, int> mappers::AIStrategyMapper::getAdmStrategies(const V3::Country& country, const V3::ClayManager& clayManager) const
{
	return filterStrategies(admStrategies, defaultAdmStrategy, country, clayManager);
}

std::map<std::string, int> mappers::AIStrategyMapper::getDipStrategies(const V3::Country& country, const V3::ClayManager& clayManager) const
{
	return filterStrategies(dipStrategies, defaultDipStrategy, country, clayManager);
}

std::map<std::string, int> mappers::AIStrategyMapper::getPolStrategies(const V3::Country& country, const V3::ClayManager& clayManager) const
{
	return filterStrategies(polStrategies, defaultPolStrategy, country, clayManager);
}

std::map<std::string, int> mappers::AIStrategyMapper::filterStrategies(const std::map<std::string, AIStrategyMapping>& strategies,
	 std::string defaultStrategy,
	 const V3::Country& country,
	 const V3::ClayManager& clayManager)
{
	// Matching strategies is a map of various strategies that seem likely. AI will pick some of them at random according to their values (which are random
	// weights)

	std::map<std::string, int> matchingStrategies;

	for (const auto& [strategyName, strategy]: strategies)
	{
		auto value = strategy.matchStrategy(country, clayManager);
		if (value > 0)
		{
			if (strategy.isOverride()) // overrides means ignore whatever else, and return this instead.
				return std::map<std::string, int>{{strategyName, 0}};
			matchingStrategies.emplace(strategyName, value);
		}
	}

	// if we matched nothing, send whatever we have preset as default for this category.
	if (matchingStrategies.empty() && !defaultStrategy.empty())
		return std::map<std::string, int>{{defaultStrategy, 0}};
	else
		return matchingStrategies;
}
