#include "ColonialRegion.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ColonialRegion::ColonialRegion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ColonialRegion::registerKeys()
{
	registerKeyword("provinces", [this](const std::string& unused, std::istream& theStream) {
		for (const auto& provinceID: commonItems::intList(theStream).getInts())
			provinces.insert(provinceID);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
