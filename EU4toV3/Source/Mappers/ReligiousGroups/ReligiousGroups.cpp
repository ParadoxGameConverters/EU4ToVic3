#include "ReligiousGroups.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligiousGroup.h"

mappers::ReligiousGroups::ReligiousGroups()
{
	LOG(LogLevel::Info) << "Parsing V2 Religions";
	registerKeys();
	parseFile("configurables/religion_definitions.txt");
	clearRegisteredKeywords();
}

mappers::ReligiousGroups::ReligiousGroups(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligiousGroups::registerKeys()
{
	registerRegex(R"([\w_]+)", [this](std::string religiousGroupName, std::istream& theStream) {
		auto newGroup = std::make_shared<ReligiousGroup>(religiousGroupName, theStream);

		if (const auto& hpm = newGroup->getHpm(); !hpm.empty())
		{
			if (!theConfiguration.isHpmEnabled() && hpm == "yes")
				return;
			if (theConfiguration.isHpmEnabled())
			{
				if (hpm == "no")
					return;
				else if (hpm != "yes")
					religiousGroupName = hpm;
			}
		}

		religiousGroupsMap.insert(std::make_pair(religiousGroupName, newGroup));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}