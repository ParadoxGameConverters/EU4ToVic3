#include "Unreleasables.h"
#include "ParserHelpers.h"

mappers::Unreleasables::Unreleasables()
{
	registerKeyword("unreleasable", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString unreleasableStr(theStream);
			unreleasables.insert(unreleasableStr.getString());
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseFile("configurables/unreleasable_tags.txt");
}

bool mappers::Unreleasables::isTagReleasable(const std::string& tag) const
{
	return !unreleasables.count(tag);
}