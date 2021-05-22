#ifndef FACTORY_INVENTION_MAPPER_H
#define FACTORY_INVENTION_MAPPER_H
#include "Parser.h"

namespace mappers
{

class FactoryInventionMapper: commonItems::parser
{
  public:
	FactoryInventionMapper();
	explicit FactoryInventionMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getInventionForFactoryType(const std::string& factoryType) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> factoryInventionMap;
};
} // namespace mappers

#endif // FACTORY_INVENTION_MAPPER_H