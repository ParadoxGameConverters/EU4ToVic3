#include "AgreementMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::AgreementMapper::AgreementMapper()
{
	Log(LogLevel::Info) << "Parsing Agreement Types.";
	registerKeys();
	parseFile("configurables/agreement_types.txt");
	clearRegisteredKeywords();
}

mappers::AgreementMapper::AgreementMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AgreementMapper::registerKeys()
{
	registerKeyword("colonies", [this](const std::string& unused, std::istream& theStream) {
		const auto theList = commonItems::stringList(theStream).getStrings();
		colonies.insert(theList.begin(), theList.end());
	});
	registerKeyword("onesiders", [this](const std::string& unused, std::istream& theStream) {
		const auto theList = commonItems::stringList(theStream).getStrings();
		onesiders.insert(theList.begin(), theList.end());
	});
	registerKeyword("doublesiders", [this](const std::string& unused, std::istream& theStream) {
		const auto theList = commonItems::stringList(theStream).getStrings();
		doublesiders.insert(theList.begin(), theList.end());
	});
	registerKeyword("tributaries", [this](const std::string& unused, std::istream& theStream) {
		const auto theList = commonItems::stringList(theStream).getStrings();
		tributaries.insert(theList.begin(), theList.end());
	});
	registerKeyword("vassals", [this](const std::string& unused, std::istream& theStream) {
		const auto theList = commonItems::stringList(theStream).getStrings();
		vassals.insert(theList.begin(), theList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
