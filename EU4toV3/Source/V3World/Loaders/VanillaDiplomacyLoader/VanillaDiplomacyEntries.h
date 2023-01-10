#ifndef VANILLA_DIPLOMACY_ENTRIES_H
#define VANILLA_DIPLOMACY_ENTRIES_H
#include "Parser.h"
#include "VanillaAgreementEntry.h"
#include "VanillaRelationEntry.h"
#include "VanillaTruceEntry.h"

namespace V3
{
class VanillaDiplomacyEntries: commonItems::parser
{
  public:
	VanillaDiplomacyEntries() = default;
	explicit VanillaDiplomacyEntries(std::istream& theStream);

	[[nodiscard]] const auto& getAgreementEntries() const { return agreementEntries; }
	[[nodiscard]] const auto& getRelationEntries() const { return relationEntries; }
	[[nodiscard]] const auto& getTruceEntries() const { return truceEntries; }

  private:
	void registerKeys();

	parser entryParser;
	std::string activeTag;

	std::vector<VanillaAgreementEntry> agreementEntries;
	std::vector<VanillaRelationEntry> relationEntries;
	std::vector<VanillaTruceEntry> truceEntries;
};
} // namespace V3
#endif // VANILLA_DIPLOMACY_ENTRIES_H