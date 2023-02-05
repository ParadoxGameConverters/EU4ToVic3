#include "EU4Relations.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4Relations::EU4Relations(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Relations::registerKeys()
{
	registerRegex("[A-Z0-9]{3}", [this](const std::string& tag, std::istream& theStream) {
		const EU4RelationDetails details(theStream);
		relations.emplace(tag, details);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
