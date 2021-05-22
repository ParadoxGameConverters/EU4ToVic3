#ifndef FACTORY_TYPE_LOADER_H
#define FACTORY_TYPE_LOADER_H
#include "FactoryCounts/FactoryStartingCounts.h"
#include "FactoryInventions/FactoryInventionMapper.h"
#include "FactoryTechs/FactoryTechMapper.h"
#include "FactoryType.h"
#include "Parser.h"
#include "V3World/Factory/Factory.h"
#include <deque>

namespace mappers
{
class FactoryTypeMapper: commonItems::parser
{
  public:
	FactoryTypeMapper();
	explicit FactoryTypeMapper(std::istream& mainStream, std::istream& techsStream, std::istream& inventionsStream, std::istream& countsStream);

	[[nodiscard]] std::deque<std::shared_ptr<V2::Factory>> buildFactories() const;

  private:
	void registerKeys();
	void assignFactories();

	std::vector<FactoryType> factoryTypes;
	std::unique_ptr<FactoryStartingCounts> factoryStartingCounts;
	std::unique_ptr<FactoryTechMapper> factoryTechs;
	std::unique_ptr<FactoryInventionMapper> factoryInventions;
};
} // namespace mappers

#endif // FACTORY_TYPE_LOADER_H