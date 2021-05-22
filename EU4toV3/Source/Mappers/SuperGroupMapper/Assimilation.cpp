#include "Assimilation.h"
#include "ParserHelpers.h"

mappers::Assimilation::Assimilation(std::istream& theStream)
{
	registerKeyword("assimilation", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt assimilationInt(theStream);
			assimilation = assimilationInt.getInt();
		});
	registerRegex("[a-zA-Z0-9\\_]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}