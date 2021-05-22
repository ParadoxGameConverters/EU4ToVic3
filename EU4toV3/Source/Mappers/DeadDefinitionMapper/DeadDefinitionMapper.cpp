#include "DeadDefinitionMapper.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::DeadDefinitionMapper::DeadDefinitionMapper()
{
	registerKeys();
	parseFile("configurables/dead_definitions.txt");
	clearRegisteredKeywords();
}

mappers::DeadDefinitionMapper::DeadDefinitionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::DeadDefinitionMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const DeadDefinitionMapping newDead(theStream);
		if (newDead.getDefinition().tag.empty())
			throw std::runtime_error("Dead Definition has no tag!");
		deadDefinitions.insert(std::pair(newDead.getDefinition().tag, newDead.getDefinition()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::DeadDefinition> mappers::DeadDefinitionMapper::getDeadDefinitionForTag(const std::string& tag) const
{
	const auto& definitionItr = deadDefinitions.find(tag);
	if (definitionItr != deadDefinitions.end())
		return definitionItr->second;
	else
		return std::nullopt;
}
