#include "EU4GovernmentSection.h"
#include "CommonRegexes.h"
#include "EU4ReformStackSection.h"
#include "ParserHelpers.h"

EU4::GovernmentSection::GovernmentSection(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::GovernmentSection::registerKeys()
{
	registerSetter("government", government);
	registerKeyword("reform_stack", [this](std::istream& theStream) {
		const ReformStackSection refStack(theStream);
		reforms = refStack.getReforms();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
