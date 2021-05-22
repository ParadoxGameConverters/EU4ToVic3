#include "EU4Leader.h"
#include "EU4LeaderID.h"
#include "Log.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::Leader::Leader(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Leader::registerKeys()
{
	registerSetter("name", name);
	registerSetter("type", leaderType);
	registerKeyword("female", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		female = true;
	});
	registerSetter("manuever", maneuver);
	registerSetter("fire", fire);
	registerSetter("shock", shock);
	registerSetter("siege", siege);
	registerKeyword("activation", [this](std::istream& theStream) {
		activationDate = date(commonItems::getString(theStream));
	});
	registerKeyword("id", [this](std::istream& theStream) {
		const LeaderID theID(theStream);
		leaderID = theID.getIDNum();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool EU4::Leader::isLand() const
{
	if (leaderType == "general" || leaderType == "conquistador")
		return true;
	if (leaderType == "explorer" || leaderType == "admiral")
		return false;

	LOG(LogLevel::Warning) << "Unknown leader type " << leaderType;
	return false;
}
