#ifndef EU4WAR_H
#define EU4WAR_H
#include "EU4WarDetails.h"
#include "Parser.h"

namespace EU4
{
class War: commonItems::parser
{
  public:
	explicit War(std::istream& theStream);

	[[nodiscard]] const auto& getDetails() const { return details; }
	[[nodiscard]] const auto& getAttackers() const { return attackers; }
	[[nodiscard]] const auto& getDefenders() const { return defenders; }
	[[nodiscard]] const auto& getName() const { return name; }

  private:
	void registerKeys();

	WarDetails details;
	std::string name;
	std::vector<std::string> attackers; // order maters! first is primary.
	std::vector<std::string> defenders; // order maters! first is primary.
};
} // namespace EU4

#endif // EU4WAR_H