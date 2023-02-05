#ifndef EU4_LEADER_ID
#define EU4_LEADER_ID
#include "ConvenientParser.h"

namespace EU4
{
class LeaderID: commonItems::convenientParser
{
  public:
	LeaderID() = default;
	explicit LeaderID(std::istream& theStream);

	[[nodiscard]] const auto& getIDNum() const { return IDNum; }

  private:
	void registerKeys();

	int IDNum = 0;
};
} // namespace EU4

#endif // EU4_LEADER_ID