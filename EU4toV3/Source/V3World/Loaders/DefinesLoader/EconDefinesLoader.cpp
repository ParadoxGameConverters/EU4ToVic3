#include "EconDefinesLoader.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void V3::EconDefinesLoader::loadEconDefines(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void V3::EconDefinesLoader::registerKeys()
{
	registerKeyword("global_construction_points", [this](std::istream& theStream) {
		globalCP = commonItems::getInt(theStream);
	});
	registerKeyword("centralized_population_ratio", [this](std::istream& theStream) {
		centralizedPopRatio = commonItems::getDouble(theStream);
	});
	registerKeyword("average_industrial_score", [this](std::istream& theStream) {
		meanIndustrialScore = commonItems::getDouble(theStream);
	});
	registerKeyword("state_trait_strength", [this](std::istream& theStream) {
		stateTraitStrength = commonItems::getDouble(theStream);
	});
	registerKeyword("packet_factor", [this](std::istream& theStream) {
		packetFactor = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
