#ifndef PRODUCTION_METHOD_GROUP_LOADER_H
#define PRODUCTION_METHOD_GROUP_LOADER_H

#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
#include "Parser.h"


namespace V3
{
class ProductionMethodGroupLoader: commonItems::parser
{
  public:
	ProductionMethodGroupLoader() = default;

	void loadPMGroups(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getPMGroups() const { return PMGroups; }

  private:
	void registerKeys();

	std::map<std::string, ProductionMethodGroup> PMGroups;
};
} // namespace V3
#endif // PRODUCTION_METHOD_GROUP_LOADER_H