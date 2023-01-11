#include "VanillaDiplomaticPlayEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V3::VanillaDiplomaticPlayEntries::VanillaDiplomaticPlayEntries(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaDiplomaticPlayEntries::registerKeys()
{
	registerRegex(R"([c,C]:\w+)", [this](const std::string& sourceTag, std::istream& theStream) {
		activeTag = sourceTag.substr(2, sourceTag.size());
		entryParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	entryParser.registerRegex(commonItems::catchallRegex, [this](const std::string& elementName, std::istream& theStream) {
		const auto elementValue = commonItems::stringOfItem(theStream);
		diploPlayElements[activeTag].emplace_back(elementName + " " + elementValue.getString());
	});
}
