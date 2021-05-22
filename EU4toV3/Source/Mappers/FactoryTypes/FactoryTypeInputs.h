#ifndef FACTORY_TYPE_INPUTS_H
#define FACTORY_TYPE_INPUTS_H
#include "Parser.h"

namespace mappers
{
class FactoryTypeInputs: commonItems::parser
{
  public:
	explicit FactoryTypeInputs(std::istream& theStream);

	[[nodiscard]] const auto& getProductionInputValues() const { return productionInputValues; }

  private:
	void registerKeys();

	std::map<std::string, double> productionInputValues;
};
} // namespace mappers

#endif // FACTORY_TYPE_INPUTS_H