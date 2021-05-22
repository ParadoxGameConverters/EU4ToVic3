#ifndef PARTY_TYPE_H
#define PARTY_TYPE_H
#include "Date.h"
#include "Parser.h"

namespace mappers
{
class PartyType: commonItems::parser
{
  public:
	PartyType() = default;
	explicit PartyType(std::istream& theStream);

	void setName(const std::string& partyName) { name = partyName; }
	void setIdeology(const std::string& theIdeology) { ideology = theIdeology; }
	void setStartDate(const std::string& startDateString) { startDate = date(startDateString); }
	void addPolicy(const std::string& policy, const std::string& position) { policies.insert(make_pair(policy, position)); }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getIdeology() const { return ideology; }
	[[nodiscard]] const auto& getStartDate() const { return startDate; }
	[[nodiscard]] const auto& getEndDate() const { return endDate; }
	[[nodiscard]] const auto& getPolicies() const { return policies; }
	[[nodiscard]] const auto& getPolicyPosition(const std::string& policy) const { return policies.find(policy)->second; }

	friend std::ostream& operator<<(std::ostream& output, const PartyType& partyDetails);

  private:
	void registerKeys();

	std::string name;
	std::string ideology;
	
	date startDate;
	date endDate;

	std::map<std::string, std::string> policies; // policyName / policy position
};
} // namespace mappers

#endif // PARTY_TYPE_H