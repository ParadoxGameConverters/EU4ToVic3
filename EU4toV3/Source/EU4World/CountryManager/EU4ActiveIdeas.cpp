#include "EU4ActiveIdeas.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4ActiveIdeas::EU4ActiveIdeas(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4ActiveIdeas::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& ideaName, std::istream& theStream) {
		// Don't bother with incomplete ideas.
		if (commonItems::getInt(theStream) >= 7)
			activeIdeas.insert(ideaName);
	});
}
