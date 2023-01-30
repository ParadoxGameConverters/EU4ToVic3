#ifndef PRODUCTION_METHOD_MAPPER_H
#define PRODUCTION_METHOD_MAPPER_H
#include "Parser.h"
#include "ProductionMethodMapping.h"

namespace V3
{
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
	void applyRules(const std::shared_ptr<V3::Country>& country,
		 const std::map<std::string, V3::ProductionMethod>& PMs,
		 const std::map<std::string, V3::ProductionMethodGroup>& PMGroups) const;

	[[nodiscard]] const auto& getRules() const { return buildingToRules; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<PMRule>> buildingToRules; // building_name -> PMRules
};
} // namespace mappers

#endif // PRODUCTION_METHOD_MAPPER_H