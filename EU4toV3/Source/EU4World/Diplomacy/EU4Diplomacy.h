#ifndef EU4_DIPLOMACY_H
#define EU4_DIPLOMACY_H
#include "EU4Agreement.h"
#include "Parser.h"

namespace EU4
{
class EU4Diplomacy: commonItems::parser
{
  public:
	EU4Diplomacy() = default;
	explicit EU4Diplomacy(std::istream& theStream);

	[[nodiscard]] const auto& getAgreements() const { return agreements; }

  private:
	void registerKeys();

	std::vector<EU4Agreement> agreements;
};
} // namespace EU4

#endif // EU4DIPLOMACY_H