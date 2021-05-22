#include "MinorityPopMapper.h"
#include "MinorityPop.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::MinorityPopMapper::MinorityPopMapper()
{
	registerKeys();
	parseFile("configurables/minority_pops.txt");
	clearRegisteredKeywords();
}

mappers::MinorityPopMapper::MinorityPopMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::MinorityPopMapper::registerKeys()
{
	registerKeyword("minority", [this](const std::string& unused, std::istream& theStream) {
		const MinorityPop minPop(theStream);
		minorityPopMap.emplace_back(make_pair(minPop.getCulture(), minPop.getReligion()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::MinorityPopMapper::blankMajorityFromMinority(V2::Pop& pop) const
{
	// We are blanking majority part from a minority pop so that we may apply any
	// religion to gipsies or any culture to mormons.
	for (const auto& [minorityCulture, minorityReligion]: minorityPopMap)
	{
		if (pop.getCulture() == minorityCulture && pop.getReligion() == minorityReligion)
			return true;

		if (minorityCulture.empty() && pop.getReligion() == minorityReligion)
		{
			pop.blankCulture();
			return true;
		}

		if (pop.getCulture() == minorityCulture && minorityReligion.empty())
		{
			pop.blankReligion();
			return true;
		}
	}
	return false;
}
