#include "FactoryType.h"
#include "FactoryTypeInputs.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryType::FactoryType(std::istream& theStream, std::string theName): factoryTypeName(std::move(theName))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryType::registerKeys()
{
	registerKeyword("output_goods", [this](const std::string& unused, std::istream& theStream) {
		outputs = commonItems::singleString(theStream).getString();
	});
	registerKeyword("is_coastal", [this](const std::string& unused, std::istream& theStream) {
		coastal = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("input_goods", [this](const std::string& unused, std::istream& theStream) {
		const FactoryTypeInputs v2Inputs(theStream);
		inputs = v2Inputs.getProductionInputValues();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::FactoryType::assignTechsAndCounts(const std::optional<std::string>& theRequiredTech,
	 const std::optional<std::string>& theRequiredInvention,
	 const std::optional<int>& theStartingCount)
{
	if (theRequiredTech)
		requiredTech = *theRequiredTech;

	if (theRequiredInvention)
		requiredInvention = *theRequiredInvention;

	if (theStartingCount)
		startingCount = *theStartingCount;
}
