#include "ProductionMethod.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::ProductionMethod::loadProductionMethod(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::ProductionMethod::registerKeys()
{
	scalingParser.registerKeyword("country_bureaucracy_add", [this](std::istream& theStream) {
		bureaucracy = commonItems::getInt(theStream);
	});
	scalingParser.registerRegex("goods_input\\w+_add", [this](const std::string& goodsType, std::istream& theStream) {
		inputs.emplace(getType(goodsType), commonItems::getDouble(theStream));
	});
	scalingParser.registerRegex("goods_output\\w+_add", [this](const std::string& goodsType, std::istream& theStream) {
		outputs.emplace(getType(goodsType), commonItems::getDouble(theStream));
	});
	scalingParser.registerRegex("building_employment_\\w+_add", [this](const std::string& employmentType, std::istream& theStream) {
		employment.emplace(getType(employmentType), commonItems::getInt(theStream));
	});
	scalingParser.IgnoreUnregisteredItems();

	modifiersParser.registerKeyword("workforce_scaled", [this](std::istream& theStream) {
		scalingParser.parseStream(theStream);
	});
	modifiersParser.registerKeyword("level_scaled", [this](std::istream& theStream) {
		scalingParser.parseStream(theStream);
	});
	modifiersParser.IgnoreUnregisteredItems();

	registerKeyword("unlocking_technologies", [this](std::istream& theStream) {
		for (const auto& tech: commonItems::getStrings(theStream))
			unlockingTechs.emplace(tech);
	});
	registerKeyword("unlocking_laws", [this](std::istream& theStream) {
		for (const auto& law: commonItems::getStrings(theStream))
			unlockingLaws.emplace(law);
	});
	registerKeyword("disallowing_laws", [this](std::istream& theStream) {
		for (const auto& law: commonItems::getStrings(theStream))
			disallowingLaws.emplace(law);
	});
	registerKeyword("country_modifiers", [this](std::istream& theStream) {
		modifiersParser.parseStream(theStream);
	});
	registerKeyword("building_modifiers", [this](std::istream& theStream) {
		modifiersParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::string V3::ProductionMethod::getType(const std::string& typeString)
{
	std::stringstream typeStream(typeString);
	std::string type;

	for (int i = 0; i < 3; i++)
	{
		std::getline(typeStream, type, '_');
	}

	return type;
}
