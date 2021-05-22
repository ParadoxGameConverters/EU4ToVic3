#ifndef EU4_LEADER_ID_H
#define EU4_LEADER_ID_H
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

#endif // EU4_LEADER_ID_H