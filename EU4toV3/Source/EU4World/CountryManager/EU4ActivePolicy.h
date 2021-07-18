#ifndef EU4ACTIVE_POLICY_H
#define EU4ACTIVE_POLICY_H
#include "ConvenientParser.h"

namespace EU4
{
class EU4ActivePolicy: commonItems::convenientParser
{
  public:
	explicit EU4ActivePolicy(std::istream& theStream);

	[[nodiscard]] const auto& getPolicy() const { return policy; }

  private:
	void registerKeys();

	std::string policy;
};
} // namespace EU4

#endif // EU4ACTIVE_POLICY_H