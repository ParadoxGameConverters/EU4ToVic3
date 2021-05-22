#include "EU4GovernmentSection.h"
#include "EU4ReformStackSection.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::GovernmentSection::GovernmentSection(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::GovernmentSection::registerKeys()
{
	registerKeyword("government", [this](const std::string& unused, std::istream& theStream) {
		government = commonItems::singleString(theStream).getString();
	});
	registerKeyword("reform_stack", [this](const std::string& unused, std::istream& theStream) {
		const ReformStackSection refStack(theStream);
		reformStack = refStack.getReforms();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
