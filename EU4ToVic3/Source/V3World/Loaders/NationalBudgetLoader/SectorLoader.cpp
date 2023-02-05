#include "SectorLoader.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"

void V3::SectorLoader::loadSector(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

double V3::SectorLoader::calculateWeight(const Country& country) const
{
	double theWeight = weight;
	theWeight = mergeValidAdds(country) + theWeight;
	theWeight = mergeValidMults(country) * theWeight;

	// Severely penalize unrecognized countries for these sectors
	if (industrial) // TODO(Gawquon) Might want to create 2 stages of this? Circle back after seeing it in action.
	{
		theWeight *= country.getProcessedData().civLevel / 100.0 * country.getIndustryFactor();
	}

	return theWeight;
}

void V3::SectorLoader::registerKeys()
{
	registerKeyword("industrial", [this](std::istream& theStream) {
		if (commonItems::getString(theStream) == "yes")
			industrial = true;
	});
	registerKeyword("weight", [this](std::istream& theStream) {
		weight = commonItems::getDouble(theStream);
	});
	registerKeyword("add", [this](std::istream& theStream) {
		AddScript newAdder;
		newAdder.loadMathScript(theStream);
		adders.push_back(newAdder);
	});
	registerKeyword("multiply", [this](std::istream& theStream) {
		MultiplyScript newMulti;
		newMulti.loadMathScript(theStream);
		multipliers.push_back(newMulti);
	});
	registerRegex(R"(building_\w+)", [this](const std::string& building, std::istream& theStream) {
		buildings.emplace(building);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

V3::AddScript V3::SectorLoader::mergeValidAdds(const Country& country) const
{
	std::vector<AddScript> validAdds;
	for (const auto& add: adders)
	{
		if (add.isValid(country))
		{
			validAdds.push_back(add);
		}
	}
	return AddScript::combine(validAdds);
}

V3::MultiplyScript V3::SectorLoader::mergeValidMults(const Country& country) const
{
	std::vector<MultiplyScript> validMults;
	for (const auto& mult: multipliers)
	{
		if (mult.isValid(country))
		{
			validMults.push_back(mult);
		}
	}
	return MultiplyScript::combine(validMults);
}
