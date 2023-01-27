#ifndef DIPLOMACY_PARSER
#define DIPLOMACY_PARSER
#include "EU4Agreement.h"
#include "Parser.h"

namespace EU4
{
class DiplomacyParser: commonItems::parser
{
  public:
	DiplomacyParser() = default;

	void loadDiplomacy(std::istream& theStream);

	[[nodiscard]] const auto& getAgreements() const { return agreements; }
	void addAgreement(const EU4Agreement& agreement) { agreements.emplace_back(agreement); }

  private:
	void registerKeys();

	std::vector<EU4Agreement> agreements;
};
} // namespace EU4

#endif // DIPLOMACY_PARSER