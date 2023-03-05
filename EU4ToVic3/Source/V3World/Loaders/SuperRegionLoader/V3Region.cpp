#include "V3Region.h"
#include "ClayManager/State/State.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include <numeric>

void V3::Region::initializeRegion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::Region::registerKeys()
{
	registerKeyword("states", [this](std::istream& theStream) {
		for (const auto& stateName: commonItems::getStrings(theStream))
			states.emplace(stateName, nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

double V3::Region::getTotalSubStateWeight() const
{
	return std::accumulate(states.begin(), states.end(), 0.0, [](double sum, const auto& state) {
		return sum + state.second->getTotalSubStateWeight();
	});
}