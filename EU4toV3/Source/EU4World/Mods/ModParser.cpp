#include "ModParser.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::ModParser::ModParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (!path.empty())
	{
		const auto ending = getExtension(path);
		compressed = ending == "zip" || ending == "bin";
	}
}

void EU4::ModParser::registerKeys()
{
	registerSetter("name", name);
	registerRegex("path|archive", [this](const std::string& unused, std::istream& theStream) {
		path = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
