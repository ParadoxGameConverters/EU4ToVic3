#include "VanillaStateEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaStateEntries::VanillaStateEntries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaStateEntries::registerKeys()
{
	registerRegex(R"([s,S]:\w+)", [this](const std::string& theStateName, std::istream& theStream) {
		auto state = VanillaStateEntry(theStream);
		auto stateName = theStateName.substr(2, theStateName.size());
		stateEntries.emplace(stateName, state);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
