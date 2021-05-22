#include "ReligionMapping.h"
#include "ParserHelpers.h"

mappers::ReligionMapping::ReligionMapping(std::istream& theStream)
{
	registerKeyword("v2", [this](const std::string & unused, std::istream & theStream)
	{
		const commonItems::singleString religionString(theStream);
		vic2Religion = religionString.getString();
	});
	registerKeyword("eu4", [this](const std::string & unused, std::istream & theStream)
	{
			const commonItems::singleString religionString(theStream);
		eu4Religions.push_back(religionString.getString());
	});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}