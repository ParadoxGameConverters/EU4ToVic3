#include "SubStateEntries.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "SubStatePopEntries.h"

void V3::SubStateEntries::loadSubStates(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::SubStateEntries::registerKeys()
{
	registerRegex(R"(region_state:\w+)", [this](const std::string& subStateName, std::istream& theStream) {
		SubStatePopEntries subStatePopEntries;
		subStatePopEntries.loadPops(theStream);
		SubStatePops theSubStatePops;
		theSubStatePops.setPops(subStatePopEntries.getPops());
		if (getTagFromName(subStateName))
			theSubStatePops.setTag(*getTagFromName(subStateName));
		else
			Log(LogLevel::Warning) << "Incoming pops within substate " << subStateName << " have no parsable TAG. Filing pops as unclaimed.";
		subStatePops.push_back(theSubStatePops);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> V3::SubStateEntries::getTagFromName(const std::string& name)
{
	const auto colonPos = name.find_last_of(':');
	if (colonPos == std::string::npos || colonPos >= name.size() - 3) // save room for the tag.
		return std::nullopt;
	return name.substr(colonPos + 1, name.size());
}
