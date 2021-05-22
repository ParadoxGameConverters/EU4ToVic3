#include "EU4ReformStackSection.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ReformStackSection::ReformStackSection(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ReformStackSection::registerKeys()
{
	registerKeyword("reforms", [this](const std::string& unused, std::istream& theStream) {
		for (const auto& reform: commonItems::stringList(theStream).getStrings())
			reforms.insert(reform);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
