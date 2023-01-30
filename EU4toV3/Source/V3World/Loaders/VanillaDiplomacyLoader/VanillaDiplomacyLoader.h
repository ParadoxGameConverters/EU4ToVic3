#ifndef VANILLA_DIPLOMACY_LOADER_H
#define VANILLA_DIPLOMACY_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "VanillaAgreementEntry.h"
#include "VanillaRelationEntry.h"
#include "VanillaTruceEntry.h"

namespace V3
{
class VanillaDiplomacyLoader: commonItems::parser
{
  public:
	VanillaDiplomacyLoader() = default;
	void loadVanillaDiplomacy(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getAgreementEntries() const { return agreementEntries; }
	[[nodiscard]] const auto& getRelationEntries() const { return relationEntries; }
	[[nodiscard]] const auto& getTruceEntries() const { return truceEntries; }

  private:
	void registerKeys();

	std::vector<VanillaAgreementEntry> agreementEntries;
	std::vector<VanillaRelationEntry> relationEntries;
	std::vector<VanillaTruceEntry> truceEntries;
};
} // namespace V3
#endif // VANILLA_DIPLOMACY_LOADER_H