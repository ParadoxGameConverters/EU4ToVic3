#ifndef VANILLA_AGREEMENT_ENTRY_H
#define VANILLA_AGREEMENT_ENTRY_H
#include "Parser.h"

namespace V3
{
class VanillaAgreementEntry: commonItems::parser
{
  public:
	VanillaAgreementEntry() = default;
	explicit VanillaAgreementEntry(std::istream& theStream, std::string sourceTag);

	[[nodiscard]] const auto& getSourceTag() const { return sourceTag; }
	[[nodiscard]] const auto& getTargetTag() const { return targetTag; }
	[[nodiscard]] const auto& getAgreementType() const { return agreementType; }

  private:
	void registerKeys();

	std::string sourceTag;
	std::string targetTag;
	std::string agreementType;
};
} // namespace V3
#endif // VANILLA_AGREEMENT_ENTRY_H