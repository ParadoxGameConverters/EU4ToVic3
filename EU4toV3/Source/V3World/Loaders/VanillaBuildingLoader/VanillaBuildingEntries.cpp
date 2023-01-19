#include "VanillaBuildingEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaBuildingEntries::VanillaBuildingEntries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaBuildingEntries::registerKeys()
{
	registerRegex(R"(region_state:\w+)", [this](const std::string& sourceTag, std::istream& theStream) {
		activeTag = sourceTag.substr(13, sourceTag.size());
		entryParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	entryParser.registerRegex(commonItems::catchallRegex, [this](const std::string& elementName, std::istream& theStream) {
		const auto elementValue = commonItems::stringOfItem(theStream);
		buildingElements[activeTag].emplace_back(elementName + " " + elementValue.getString());
	});
}
