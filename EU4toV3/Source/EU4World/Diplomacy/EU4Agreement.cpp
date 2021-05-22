#include "EU4Agreement.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4Agreement::EU4Agreement(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Agreement::registerKeys()
{
	registerRegex("subject_type|type", [this](const std::string& unused, std::istream& theStream) {
		agreementType = commonItems::singleString(theStream).getString();
	});
	registerKeyword("start_date", [this](const std::string& unused, std::istream& theStream) {
		startDate = date(commonItems::singleString(theStream).getString());
	});
	registerKeyword("end_date", [this](const std::string& unused, std::istream& theStream) {
		endDate = date(commonItems::singleString(theStream).getString());
	});
	registerKeyword("first", [this](const std::string& unused, std::istream& theStream) {
		originTag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("second", [this](const std::string& unused, std::istream& theStream) {
		targetTag = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
