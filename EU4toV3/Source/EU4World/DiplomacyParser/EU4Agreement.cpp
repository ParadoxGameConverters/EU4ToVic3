#include "EU4Agreement.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4Agreement::EU4Agreement(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Agreement::registerKeys()
{
	registerRegex("subject_type|type", [this](const std::string& unused, std::istream& theStream) {
		agreementType = commonItems::getString(theStream);
	});
	registerKeyword("start_date", [this](std::istream& theStream) {
		startDate = date(commonItems::getString(theStream));
	});
	registerKeyword("end_date", [this](std::istream& theStream) {
		endDate = date(commonItems::getString(theStream));
	});
	registerKeyword("first", [this](std::istream& theStream) {
		originTag = commonItems::getString(theStream);
	});
	registerKeyword("second", [this](std::istream& theStream) {
		targetTag = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
