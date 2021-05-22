#include "StartingInventionDetails.h"
#include "ParserHelpers.h"

mappers::StartingInventionDetails::StartingInventionDetails(std::istream& theStream)
{
	registerRegex("OR|NOT|year", commonItems::ignoreItem);
	registerRegex("[a-z_]+", [this](const std::string& tech, std::istream& theStream)
		{
			// every invention links to a single tech, with the exception of vacuum_tube which is
			// irrelevant (not available at start) - so we ignore that minor flaw.
			commonItems::ignoreItem(tech, theStream);
			techName = tech;
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}
