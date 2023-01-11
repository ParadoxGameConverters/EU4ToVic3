#include "VanillaCharacterEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaCharacterEntries::VanillaCharacterEntries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaCharacterEntries::registerKeys()
{
	registerRegex(R"([c,C]:\w+)", [this](const std::string& sourceTag, std::istream& theStream) {
		activeTag = sourceTag.substr(2, sourceTag.size());
		entryParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	entryParser.registerRegex(commonItems::catchallRegex, [this](const std::string& elementName, std::istream& theStream) {
		const auto elementValue = commonItems::stringOfItem(theStream);
		characterElements[activeTag].emplace_back(elementName + " " + elementValue.getString());
	});
}
