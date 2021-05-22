#ifndef FACTORY_TECH_MAPPER_H
#define FACTORY_TECH_MAPPER_H
#include "Parser.h"

namespace mappers
{
class FactoryTechMapper: commonItems::parser
{
  public:
	FactoryTechMapper();
	explicit FactoryTechMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getTechForFactoryType(const std::string& factoryType) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> factoryTechMap;
};
} // namespace mappers

#endif // FACTORY_TECH_MAPPER_H