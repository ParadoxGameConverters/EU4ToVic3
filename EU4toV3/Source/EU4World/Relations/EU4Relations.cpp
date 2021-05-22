#include "EU4Relations.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4Relations::EU4Relations(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Relations::registerKeys()
{
	registerRegex("[A-Z0-9]{3}", [this](const std::string& tag, std::istream& theStream) {
		const EU4RelationDetails newDetails(theStream);
		relations.insert(std::make_pair(tag, newDetails));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
