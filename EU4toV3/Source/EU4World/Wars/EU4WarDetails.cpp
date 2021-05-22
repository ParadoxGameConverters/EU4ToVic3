#include "EU4WarDetails.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

void EU4::WarDetails::addDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::WarDetails::registerKeys()
{
	registerSetter("type", warGoalType);
	registerSetter("province", targetProvinceID);
	registerSetter("tag", targetTag);
	registerRegex(R"(\d+\.\d+\.\d+)", [this](const std::string& dateString, std::istream& theStream) {
		if (!startDate.isSet())
			startDate = date(dateString);
		commonItems::ignoreItem(dateString, theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
