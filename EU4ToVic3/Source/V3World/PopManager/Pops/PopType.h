#ifndef V3_POP_TYPE_H
#define V3_POP_TYPE_H
#include "string"

namespace V3
{
class PopType: commonItems::parser
{
  public:
	PopType() = default;
	explicit PopType(std::istream& theStream);

	void setType(const std::string& theType) { type = theType; }
	void setStrata(const std::string& theStrata) { strata = theStrata; }

	[[nodiscard]] const auto& getType() const { return type; }
	[[nodiscard]] const auto& getConsumptionRate() const { return consumptionRate; }
	[[nodiscard]] const auto& getDependentRatio() const { return dependentRatio; }
	[[nodiscard]] const auto& getStrata() const { return strata; }

  private:
	void registerKeys();

	std::string type;
	double consumptionRate = 1;			  // What % of buy packages a Pop buys.
	std::optional<double> dependentRatio; // The % of working adults in a Pop. Undefined ratios use a Vic3 define.
	std::string strata;
};
} // namespace V3

#endif // V3_POP_TYPE_H