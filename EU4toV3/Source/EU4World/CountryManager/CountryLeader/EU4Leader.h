#ifndef EU4_LEADER
#define EU4_LEADER
#include "ConvenientParser.h"
#include "EU4Character.h"

namespace EU4
{
class Leader: commonItems::convenientParser
{
  public:
	Leader() = default;
	void parseLeader(std::istream& theStream);
	void parseRuler(std::istream& theStream);

	[[nodiscard]] const auto& getCharacter() const { return character; }

  private:
	void registerKeys();

	convenientParser leaderParser;
	convenientParser rulerParser;

	Character character;
};
} // namespace EU4

#endif // EU4_LEADER