#ifndef VIC3_DEFINES_LOADER_H
#define VIC3_DEFINES_LOADER_H
#include "Parser.h"

namespace V3
{
class Vic3DefinesLoader: commonItems::parser
{
  public:
	Vic3DefinesLoader() = default;

	void loadDefines(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getSplitStatePrimeLandWeight() const { return splitStatePrimeLandWeight; }
	[[nodiscard]] const auto& getIndividualsPerPopInfrastructure() const { return individualsPerPopInfrastructure; }
	[[nodiscard]] const auto& getStateBureaucracyBaseCost() const { return stateBureaucracyBaseCost; }
	[[nodiscard]] const auto& getStateBureaucracyPopBaseCost() const { return stateBureaucracyPopBaseCost; }
	[[nodiscard]] const auto& getStateBureaucracyPopMultiple() const { return stateBureaucracyPopMultiple; }
	[[nodiscard]] const auto& getMinimumInvestmentCost() const { return minimumInvestmentCost; }
	[[nodiscard]] const auto& getWorkingAdultRatioBase() const { return workingAdultRatioBase; }
	[[nodiscard]] const auto& getDependentConsumptionRatio() const { return dependentConsumptionRatio; }

  private:
	void registerKeys();
	parser nCountryParser;
	parser nPoliticsParser;
	parser nPopsParser;

	double splitStatePrimeLandWeight = 0.0;  // The weight of prime land for dividing the resources of a split state.
	int individualsPerPopInfrastructure = 0; // Multiply state_infrastructure_from_population_add by this to determine infra from population.

	int stateBureaucracyBaseCost = 0;		 // The base cost per incorporated state
	double stateBureaucracyPopBaseCost = 0; // The base BUR multiplier per population multiple
	double stateBureaucracyPopMultiple = 0; // Number of people BUR cost is based on
	double minimumInvestmentCost = 0;		 // The absolute minimum cost in BUR per investment level


	double workingAdultRatioBase = 0.25;	 // Base ratio of working adults to dependents, this can be overridden by pop type definition and country modifiers.
	double dependentConsumptionRatio = 0.0; // Dependents consume this multiple of Needs compared to Working Adults.
};
} // namespace V3
#endif // VIC3_DEFINES_LOADER_H