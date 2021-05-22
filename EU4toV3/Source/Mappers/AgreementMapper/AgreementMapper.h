#ifndef AGREEMENT_MAPPER_H
#define AGREEMENT_MAPPER_H
#include "Parser.h"
#include <set>

namespace mappers
{
class AgreementMapper: commonItems::parser
{
  public:
	AgreementMapper();
	explicit AgreementMapper(std::istream& theStream);

	[[nodiscard]] auto isAgreementInColonies(const std::string& subject) const { return colonies.count(subject); }
	[[nodiscard]] auto isAgreementInOnesiders(const std::string& subject) const { return onesiders.count(subject); }
	[[nodiscard]] auto isAgreementInDoublesiders(const std::string& subject) const { return doublesiders.count(subject); }
	[[nodiscard]] auto isAgreementInTributaries(const std::string& subject) const { return tributaries.count(subject); }
	[[nodiscard]] auto isAgreementInVassals(const std::string& subject) const { return vassals.count(subject); }

  private:
	void registerKeys();

	std::set<std::string> colonies;
	std::set<std::string> onesiders;
	std::set<std::string> doublesiders;
	std::set<std::string> tributaries;
	std::set<std::string> vassals;
};
} // namespace mappers

#endif // AGREEMENT_MAPPER_H
