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
	registerKeyword("vn_global_construction_points", [this](std::istream& theStream) {
		VNGlobalCP = commonItems::getInt(theStream);
	});
	registerKeyword("centralized_population_ratio", [this](std::istream& theStream) {
		centralizedPopRatio = commonItems::getDouble(theStream);
	});
	registerKeyword("mean_civlevel", [this](std::istream& theStream) {
		meanCivlevel = commonItems::getDouble(theStream);
	});
	registerKeyword("state_trait_strength", [this](std::istream& theStream) {
		stateTraitStrength = commonItems::getDouble(theStream);
	});
	registerKeyword("packet_factor", [this](std::istream& theStream) {
		packetFactor = commonItems::getDouble(theStream);
	});
	registerKeyword("unincorporated_penalty", [this](std::istream& theStream) {
		incorpModifier = commonItems::getDouble(theStream);
	});
	registerKeyword("min_dev_per_pop", [this](std::istream& theStream) {
		minDevPerPop = commonItems::getDouble(theStream);
	});
	registerKeyword("max_dev_per_pop", [this](std::istream& theStream) {
		maxDevPerPop = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
