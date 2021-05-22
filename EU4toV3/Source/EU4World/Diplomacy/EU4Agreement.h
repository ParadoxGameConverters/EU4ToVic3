#ifndef EU4_AGREEMENT_H
#define EU4_AGREEMENT_H
#include "Date.h"
#include "Parser.h"

namespace EU4
{
class EU4Agreement: commonItems::parser
{
  public:
	explicit EU4Agreement(std::istream& theStream);

	void setAgreementType(const std::string& _type) { agreementType = _type; }
	void setTargetTag(const std::string& tag) { targetTag = tag; }

	[[nodiscard]] const auto& getAgreementType() const { return agreementType; }
	[[nodiscard]] const auto& getOriginTag() const { return originTag; }
	[[nodiscard]] const auto& getTargetTag() const { return targetTag; }
	[[nodiscard]] const auto& getStartDate() const { return startDate; }
	[[nodiscard]] const auto& getEndDate() const { return endDate; }

  private:
	void registerKeys();

	std::string agreementType;
	std::string originTag;
	std::string targetTag;
	date startDate;
	date endDate;
};
} // namespace EU4

#endif // EU4_AGREEMENT_H