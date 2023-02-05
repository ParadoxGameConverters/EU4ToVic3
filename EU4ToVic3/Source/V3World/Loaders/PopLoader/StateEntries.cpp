#include "StateEntries.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "SubStateEntries.h"

void V3::StateEntries::loadStates(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::StateEntries::registerKeys()
{
	registerRegex(R"(s:\w+)", [this](const std::string& stateName, std::istream& theStream) {
		SubStateEntries subStateEntries;
		subStateEntries.loadSubStates(theStream);
		StatePops theStatePops;
		theStatePops.setSubStatePops(subStateEntries.getSubStatePops());
		if (const auto state = getStateFromName(stateName); state)
		{
			theStatePops.setStateName(*state);
			statePops.emplace(*state, theStatePops);
		}
		else
		{
			Log(LogLevel::Warning) << "Incoming pops within state " << stateName << " have no parsable state. Deleting Pops!";
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> V3::StateEntries::getStateFromName(const std::string& name)
{
	const auto colonPos = name.find_last_of(':');
	if (colonPos != 1 || name.size() <= 3) // needs to be second char, and single-letter states just won't cut it.
		return std::nullopt;
	return name.substr(colonPos + 1, name.size());
}
