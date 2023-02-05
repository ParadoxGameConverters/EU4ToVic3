#include "EU4ReformStackSection.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::ReformStackSection::ReformStackSection(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ReformStackSection::registerKeys()
{
	registerKeyword("reforms", [this](std::istream& theStream) {
		const auto& reformList = commonItems::stringList(theStream).getStrings();
		reforms.insert(reformList.begin(), reformList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
