#ifndef PRODUCTION_METHOD_LOADER_H
#define PRODUCTION_METHOD_LOADER_H
#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class ProductionMethodLoader: commonItems::parser
{
  public:
	ProductionMethodLoader() = default;

	void loadPMs(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getPMs() const { return PMs; }

  private:
	void registerKeys();

	std::map<std::string, ProductionMethod> PMs;
};
} // namespace V3
#endif // PRODUCTION_METHOD_LOADER_H