#include "PopInstance.h"
#include "ParserHelpers.h"
#include <cmath>
#include "CommonRegexes.h"

mappers::PopInstance::PopInstance(std::istream& theStream, const std::string& thePopType)
{
	popDetails.popType = thePopType;
	
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PopInstance::registerKeys()
{
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		popDetails.culture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		popDetails.religion = commonItems::singleString(theStream).getString();
	});
	registerKeyword("size", [this](const std::string& unused, std::istream& theStream) {
		popDetails.size = static_cast<int>(std::round(commonItems::singleDouble(theStream).getDouble()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
