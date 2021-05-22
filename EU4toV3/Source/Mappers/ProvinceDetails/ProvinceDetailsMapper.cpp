#include "ProvinceDetailsMapper.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ProvinceDetailsMapper::ProvinceDetailsMapper(const std::string& filename)
{
	registerKeys();
	parseFile(filename);
	clearRegisteredKeywords();
}

mappers::ProvinceDetailsMapper::ProvinceDetailsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceDetailsMapper::registerKeys()
{
	registerKeyword("owner", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.owner = commonItems::singleString(theStream).getString();
	});
	registerKeyword("controller", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.controller = commonItems::singleString(theStream).getString();
	});
	registerKeyword("add_core", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.cores.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("trade_goods", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.rgoType = commonItems::singleString(theStream).getString();
	});
	registerKeyword("life_rating", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.lifeRating = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("terrain", [this](const std::string& unused, std::istream& theStream) {
		const auto& terrainStr = commonItems::singleString(theStream).getString();
		if (terrainStr != "coral_island")
			provinceDetails.terrain = terrainStr;
	});
	registerKeyword("colonial", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.colonial = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("colony", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.colonyLevel = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("naval_base", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.navalBaseLevel = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("fort", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.fortLevel = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("railroad", [this](const std::string& unused, std::istream& theStream) {
		provinceDetails.railLevel = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("is_slave", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		provinceDetails.slaveState = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
