#include "VanillaSubStateEntry.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

V3::VanillaSubStateEntry::VanillaSubStateEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaSubStateEntry::registerKeys()
{
	registerKeyword("country", [this](std::istream& theStream) {
		ownerTag = commonItems::getString(theStream);
		if ((ownerTag.starts_with("c:") || ownerTag.starts_with("C:")) && ownerTag.size() == 5) // very specific about this.
			ownerTag = ownerTag.substr(2, 3);
		else
			Log(LogLevel::Warning) << "Unrecognized substate country for " << ownerTag << "!";
	});
	registerKeyword("state_type", [this](std::istream& theStream) {
		subStateType = commonItems::getString(theStream);
	});
	registerKeyword("owned_provinces", [this](std::istream& theStream) {
		const auto theProvinces = commonItems::getStrings(theStream);
		for (const auto& province: theProvinces)
			provinces.emplace(province);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
