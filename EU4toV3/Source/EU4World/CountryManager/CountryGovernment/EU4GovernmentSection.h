#ifndef EU4_GOVERNMENT_SECTION
#define EU4_GOVERNMENT_SECTION
#include "ConvenientParser.h"
#include <set>

namespace EU4
{
class GovernmentSection: commonItems::convenientParser
{
  public:
	explicit GovernmentSection(std::istream& theStream);

	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getGovernmentReforms() const { return reforms; }

  private:
	void registerKeys();

	std::string government;
	std::set<std::string> reforms;
};
} // namespace EU4

#endif // EU4_GOVERNMENT_SECTION
