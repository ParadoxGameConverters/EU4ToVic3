#include "FactoryTypeMapper.h"
#include "Configuration.h"
#include "Log.h"
#include "CommonRegexes.h"

mappers::FactoryTypeMapper::FactoryTypeMapper()
{
	LOG(LogLevel::Info) << "Determining factory allocation rules.";
	registerKeys();
	parseFile(theConfiguration.getVic2Path() + "/common/production_types.txt");
	clearRegisteredKeywords();

	factoryTechs = std::make_unique<FactoryTechMapper>();
	factoryInventions = std::make_unique<FactoryInventionMapper>();
	factoryStartingCounts = std::make_unique<FactoryStartingCounts>();
	assignFactories();
}

mappers::FactoryTypeMapper::FactoryTypeMapper(std::istream& mainStream, std::istream& techsStream, std::istream& inventionsStream, std::istream& countsStream)
{
	registerKeys();
	parseStream(mainStream);
	clearRegisteredKeywords();

	factoryTechs = std::make_unique<FactoryTechMapper>(techsStream);
	factoryInventions = std::make_unique<FactoryInventionMapper>(inventionsStream);
	factoryStartingCounts = std::make_unique<FactoryStartingCounts>(countsStream);
	assignFactories();
}

void mappers::FactoryTypeMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& factoryName, std::istream& theStream) {
		FactoryType factoryType(theStream, factoryName);
		factoryTypes.emplace_back(factoryType);
	});
}

void mappers::FactoryTypeMapper::assignFactories()
{
	for (auto& factoryType: factoryTypes)
		factoryType.assignTechsAndCounts(factoryTechs->getTechForFactoryType(factoryType.getName()),
			 factoryInventions->getInventionForFactoryType(factoryType.getName()),
			 factoryStartingCounts->getCountForFactoryType(factoryType.getName()));
}

std::deque<std::shared_ptr<V2::Factory>> mappers::FactoryTypeMapper::buildFactories() const
{
	std::deque<std::shared_ptr<V2::Factory>> factoryQueue;
	for (const auto& factoryType: factoryTypes)
		for (auto counter = 0; counter < factoryType.getStartingCount(); ++counter)
			factoryQueue.emplace_back(std::make_shared<V2::Factory>(factoryType));

	return factoryQueue;
}
