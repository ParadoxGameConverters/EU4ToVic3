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
