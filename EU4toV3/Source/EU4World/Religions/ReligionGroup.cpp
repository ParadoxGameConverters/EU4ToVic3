#include "ReligionGroup.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ReligionGroup::ReligionGroup(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ReligionGroup::registerKeys()
{
	registerKeyword("defender_of_faith", commonItems::ignoreItem);
	registerKeyword("can_form_personal_unions", commonItems::ignoreItem);
	registerKeyword("center_of_religion", commonItems::ignoreItem);
	registerKeyword("flags_with_emblem_percentage", commonItems::ignoreItem);
	registerKeyword("flag_emblem_index_range", commonItems::ignoreItem);
	registerKeyword("ai_will_propagate_through_trade", commonItems::ignoreItem);
	registerKeyword("religious_schools", commonItems::ignoreItem);
	registerKeyword("harmonized_modifier", commonItems::ignoreItem);
	registerKeyword("crusade_name", commonItems::ignoreItem);

	registerRegex(commonItems::stringRegex, [this](const std::string& religionName, std::istream& theStream) {
		commonItems::ignoreItem(religionName, theStream);
		religions.insert(religionName);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
