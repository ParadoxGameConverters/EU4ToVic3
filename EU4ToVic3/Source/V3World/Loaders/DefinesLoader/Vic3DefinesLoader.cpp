#include "Vic3DefinesLoader.h"

void V3::Vic3DefinesLoader::loadDefines(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/defines/"))
	{
		if (fileName.extension() != ".txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::Vic3DefinesLoader::registerKeys()
{
	nCountryParser.registerKeyword("SPLIT_STATE_PRIME_LAND_WEIGHT", [this](std::istream& theStream) {
		splitStatePrimeLandWeight = commonItems::getDouble(theStream);
	});
	nCountryParser.IgnoreUnregisteredItems();

	nPoliticsParser.registerKeyword("STATE_BUREAUCRACY_BASE_COST", [this](std::istream& theStream) {
		stateBureaucracyBaseCost = commonItems::getInt(theStream);
	});
	nPoliticsParser.registerKeyword("STATE_BUREAUCRACY_POP_BASE_COST", [this](std::istream& theStream) {
		stateBureaucracyPopBaseCost = commonItems::getDouble(theStream);
	});
	nPoliticsParser.registerKeyword("STATE_BUREAUCRACY_POP_MULTIPLE", [this](std::istream& theStream) {
		stateBureaucracyPopMultiple = commonItems::getDouble(theStream);
	});
	nPoliticsParser.registerKeyword("MINIMUM_INVESTMENT_COST", [this](std::istream& theStream) {
		minimumInvestmentCost = commonItems::getDouble(theStream);
	});
	nPoliticsParser.IgnoreUnregisteredItems();

	nPopsParser.registerKeyword("WORKING_ADULT_RATIO_BASE", [this](std::istream& theStream) {
		workingAdultRatioBase = commonItems::getDouble(theStream);
	});
	nPopsParser.registerKeyword("DEPENDENT_CONSUMPTION_RATIO", [this](std::istream& theStream) {
		dependentConsumptionRatio = commonItems::getDouble(theStream);
	});
	nPopsParser.registerKeyword("INDIVIDUALS_PER_POP_INFRASTRUCTURE", [this](std::istream& theStream) {
		individualsPerPopInfrastructure = commonItems::getInt(theStream);
	});
	nPopsParser.IgnoreUnregisteredItems();

	registerKeyword("NCountry", [this](std::istream& theStream) {
		nCountryParser.parseStream(theStream);
	});
	registerKeyword("NPolitics", [this](std::istream& theStream) {
		nPoliticsParser.parseStream(theStream);
	});
	registerKeyword("NPops", [this](std::istream& theStream) {
		nPopsParser.parseStream(theStream);
	});

	IgnoreUnregisteredItems();
}
