#include "MinorityPopMapper.h"
#include "CommonRegexes.h"
#include "MinorityPop.h"
#include "ParserHelpers.h"

mappers::MinorityPopMapper::MinorityPopMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::MinorityPopMapper::loadMappingRules(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void mappers::MinorityPopMapper::registerKeys()
{
	registerKeyword("minority", [this](std::istream& theStream) {
		const MinorityPop minPop(theStream);
		minorityPopMap.emplace_back(std::make_pair(minPop.getCulture(), minPop.getReligion()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::MinorityPopMapper::blankMajorityFromMinority(V3::Pop& pop) const
{
	// We are blanking majority part from a minority pop so that we may apply any
	// religion to metis or any culture to jews.
	for (const auto& [minorityCulture, minorityReligion]: minorityPopMap)
	{
		if (pop.getCulture() == minorityCulture && pop.getReligion() == minorityReligion)
		{
			pop.setMinority();
			return true;
		}

		if (minorityCulture.empty() && pop.getReligion() == minorityReligion)
		{
			pop.blankCulture();
			pop.setMinority();
			return true;
		}

		if (pop.getCulture() == minorityCulture && minorityReligion.empty())
		{
			pop.blankReligion();
			pop.setMinority();
			return true;
		}
	}
	return false;
}
