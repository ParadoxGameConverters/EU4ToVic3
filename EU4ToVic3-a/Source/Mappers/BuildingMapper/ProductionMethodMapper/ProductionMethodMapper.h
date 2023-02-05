#ifndef PRODUCTION_METHOD_MAPPER_H
#define PRODUCTION_METHOD_MAPPER_H
#include "Parser.h"
#include "ProductionMethodMapping.h"

namespace V3
{
class Building;
class ProductionMethodGroup;
class ProductionMethod;
class Country;
} // namespace V3
namespace mappers
{
class ProductionMethodMapper: commonItems::parser
{
  public:
	ProductionMethodMapper() = default;
	void loadRules(const std::string& filePath);
	void applyRules(const V3::Country& country,
		 const std::map<std::string, V3::ProductionMethod>& PMs,
		 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups) const;

	[[nodiscard]] const auto& getRules() const { return buildingToRules; }

  private:
	void registerKeys();


	// applyRules helpers
	[[nodiscard]] static std::set<std::string> getCountryBuildingTypes(const V3::Country& country);
	[[nodiscard]] static std::vector<std::shared_ptr<V3::Building>> getBuiltBuildings(const V3::Country& country, const std::string& buildingType);
	[[nodiscard]] static std::vector<std::shared_ptr<V3::Building>> pickBuildings(const std::vector<std::shared_ptr<V3::Building>>& buildings, double percent);
	[[nodiscard]] static std::string pickPM(const V3::Country& country,
		 const std::string& targetName,
		 const std::map<std::string, V3::ProductionMethod>& PMs,
		 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups);

	// Subset-sum
	static std::vector<std::shared_ptr<V3::Building>> subSetSum(const std::vector<std::shared_ptr<V3::Building>>& subSet, int targetVal);
	static std::pair<int, int> findBreakIndexAndSum(const std::vector<std::shared_ptr<V3::Building>>& subSet, int targetNumber);
	static void initializeBreakSolution(std::vector<std::vector<int>>& stateSpace, int maxVal, int breakIndex, int breakSum, int targetVal);
	static void completeBalancedOperations(std::vector<std::vector<int>>& stateSpace,
		 int maxVal,
		 int breakIndex,
		 const std::vector<std::shared_ptr<V3::Building>>& subSet);
	static std::vector<std::shared_ptr<V3::Building>> traceSteps(const std::vector<std::vector<int>>& theStates,
		 int offBy,
		 int maxVal,
		 int breakIndex,
		 const std::vector<std::shared_ptr<V3::Building>>& subSet);

	std::map<std::string, std::vector<PMRule>> buildingToRules; // building_name -> PMRules
};
} // namespace mappers

#endif // PRODUCTION_METHOD_MAPPER_H