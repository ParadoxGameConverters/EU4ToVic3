#include "ReligiousGroup.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"
#include "Religion.h"

mappers::ReligiousGroup::ReligiousGroup(std::string theName, std::istream& theStream): name(std::move(theName))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligiousGroup::registerKeys()
{
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpm = commonItems::getString(theStream);
	});
	registerRegex(R"([\w_]+)", [this](std::string religionName, std::istream& theStream) {
		auto newReligion = std::make_shared<Religion>(theStream);

		if (const auto& hpm = newReligion->getHpm(); !hpm.empty())
		{
			if (!theConfiguration.isHpmEnabled() && hpm == "yes")
				return;
			if (theConfiguration.isHpmEnabled())
			{
				if (hpm == "no")
					return;
				else if (hpm != "yes")
					religionName = hpm;
			}
		}

		religions.insert(std::make_pair(religionName, newReligion));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}