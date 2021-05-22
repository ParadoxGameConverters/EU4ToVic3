#ifndef EU4_LEADER_H
#define EU4_LEADER_H

#include "Date.h"
#include "ConvenientParser.h"

namespace EU4
{
class Leader: commonItems::convenientParser
{
  public:
	Leader() = default;
	explicit Leader(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] auto getFire() const { return fire; }
	[[nodiscard]] auto getShock() const { return shock; }
	[[nodiscard]] auto getManeuver() const { return maneuver; }
	[[nodiscard]] auto getSiege() const { return siege; }
	[[nodiscard]] auto getID() const { return leaderID; }
	[[nodiscard]] const auto& getActivationDate() const { return activationDate; }
	[[nodiscard]] auto isFemale() const { return female; }

	[[nodiscard]] bool isLand() const;

  private:
	void registerKeys();
	
	std::string name;
	std::string leaderType;
	bool female = false;
	date activationDate;
	int fire = 0;
	int shock = 0;
	int maneuver = 0;
	int siege = 0;
	int leaderID = 0;
};
} // namespace EU4

#endif // EU4_LEADER_H