#include "V3SuperRegion.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "V3Region.h"
#include <numeric>

void V3::SuperRegion::initializeSuperRegion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::SuperRegion::registerKeys()
{
	registerKeyword(commonItems::utf8BOM, [](std::istream& theStream) {
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& regionName, std::istream& theStream) {
		const auto region = std::make_shared<Region>();
		region->initializeRegion(theStream);
		region->setName(regionName);
		regions.emplace(regionName, region);
	});
}

double V3::SuperRegion::getTotalSubStateWeight() const
{
	return std::accumulate(regions.begin(), regions.end(), 0.0, [](double sum, const auto& region) {
		return sum + region.second->getTotalSubStateWeight();
	});
}