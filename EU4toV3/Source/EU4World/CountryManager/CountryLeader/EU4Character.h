#ifndef EU4_CHARACTER
#define EU4_CHARACTER
#include "Date.h"
#include <set>

namespace EU4
{
struct Character
{
	int leaderID = 0;
	int monarchID = 0;
	std::string name;
	std::string leaderName;
	std::string futureMonarchName;
	std::string leaderType;
	double heirClaim = 0;
	bool female = false;
	bool heir = false;
	bool ruler = false;
	bool consort = false;
	bool land = false;
	bool naval = false;
	date activationDate;
	date birthDate;
	int fire = 0;
	int shock = 0;
	int maneuver = 0;
	int siege = 0;
	std::string country;
	std::string countryOfOrigin;
	std::string culture;
	std::string religion;
	std::string dynasty;
	std::set<std::string> traits;
	std::string leaderTrait;
	int adm = 0;
	int dip = 0;
	int mil = 0;
};
} // namespace EU4

#endif // EU4_CHARACTER