#include "AIStrategyMapping.h"
#include "ClayManager/ClayManager.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"

mappers::AIStrategyMapping::AIStrategyMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AIStrategyMapping::registerKeys()
{
	registerKeyword("laws", [this](std::istream& theStream) {
		lawsParser.parseStream(theStream);
	});
	registerKeyword("capital", [this](std::istream& theStream) {
		capitalParser.parseStream(theStream);
	});
	registerKeyword("gp", [this](std::istream& theStream) {
		gp = commonItems::getInt(theStream);
	});
	registerKeyword("not_gp", [this](std::istream& theStream) {
		notGP = commonItems::getInt(theStream);
	});
	registerKeyword("hremperor", [this](std::istream& theStream) {
		hremperor = commonItems::getInt(theStream);
	});
	registerKeyword("cemperor", [this](std::istream& theStream) {
		cemperor = commonItems::getInt(theStream);
	});
	registerKeyword("override", [this](std::istream& theStream) {
		override = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	lawsParser.registerRegex(commonItems::catchallRegex, [this](const std::string& lawName, std::istream& theStream) {
		laws.emplace(lawName, commonItems::getInt(theStream));
	});

	capitalParser.registerRegex(commonItems::catchallRegex, [this](const std::string& regionName, std::istream& theStream) {
		capitalRegions.emplace(regionName, commonItems::getInt(theStream));
	});
}

int mappers::AIStrategyMapping::matchStrategy(const V3::Country& country, const V3::ClayManager& clayManager) const
{
	int toReturn = 0;
	for (const auto& [law, value]: laws)
		if (country.getProcessedData().laws.contains(law))
			toReturn += value;

	for (const auto& [region, value]: capitalRegions)
	{
		if (country.getProcessedData().capitalStateName.empty())
			break;
		if (clayManager.stateIsInRegion(country.getProcessedData().capitalStateName, region))
		{
			toReturn += value;
			break;
		}
	}
	if (gp > 0 && country.getSourceCountry() && country.getSourceCountry()->isGP())
		toReturn += gp;
	if (notGP > 0 && country.getSourceCountry() && !country.getSourceCountry()->isGP())
		toReturn += notGP;
	if (hremperor > 0 && country.getSourceCountry() && country.getSourceCountry()->isHREmperor())
		toReturn += hremperor;
	if (cemperor > 0 && country.getSourceCountry() && country.getSourceCountry()->isCelestialEmperor())
		toReturn += cemperor;

	return toReturn;
}
