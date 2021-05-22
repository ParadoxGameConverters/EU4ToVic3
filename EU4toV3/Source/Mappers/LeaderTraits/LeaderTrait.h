#ifndef LEADER_TRAIT_H
#define LEADER_TRAIT_H
#include "Parser.h"

namespace mappers
{
class LeaderTrait: commonItems::parser
{
  public:
	explicit LeaderTrait(std::istream& theStream, std::string theName);

	[[nodiscard]] auto getFire() const { return fire; }
	[[nodiscard]] auto getShock() const { return shock; }
	[[nodiscard]] auto getManeuver() const { return maneuver; }
	[[nodiscard]] auto getSiege() const { return siege; }
	[[nodiscard]] auto getOther() const { return other; }
	[[nodiscard]] const auto& getName() const { return name; }

	[[nodiscard]] bool matches(const int& leaderFire, const int& leaderShock, const int& leaderManeuver, const int& leaderSiege) const;

  private:
	void registerKeys();

	std::string name;
	int fire = 0;
	int shock = 0;
	int maneuver = 0;
	int siege = 0;
	int other = 0;
};
} // namespace mappers

#endif // LEADER_TRAIT_H