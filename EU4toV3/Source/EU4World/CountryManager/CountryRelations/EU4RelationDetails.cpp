#include "EU4RelationDetails.h"
#include "CommonRegexes.h"
#include "Date.h"
#include "ParserHelpers.h"
#include <cmath>

EU4::EU4RelationDetails::EU4RelationDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4RelationDetails::registerKeys()
{
	registerSetter("attitude", attitude);
	registerSetter("cached_sum", value);
	registerSetter("last_warscore", lastWarScore);
	registerKeyword("military_access", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		military_access = true;
	});
	registerKeyword("truce", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		truce = true;
	});
	registerKeyword("last_war", [this](std::istream& theStream) {
		lastWar = date(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<date> EU4::EU4RelationDetails::getTruceExpiry() const
{
	if (!truce)
		return std::nullopt;
	date expiryDate = lastWar;
	const int expiresIn = 60 + static_cast<int>(std::round(120.0 * static_cast<double>(lastWarScore) / 100.0));
	expiryDate.ChangeByMonths(expiresIn);
	return expiryDate;
}
