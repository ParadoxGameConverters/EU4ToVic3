#ifndef STARTING_TECH_MAPPER_H
#define STARTING_TECH_MAPPER_H

#include "Parser.h"
#include "StartingTech.h"

namespace mappers
{
	class StartingTechMapper: commonItems::parser
	{
	public:
		StartingTechMapper();
		
		[[nodiscard]] std::vector<std::string> getTechsForGroupAndScore(const std::string& group, double score) const;

	private:
		std::vector<StartingTech> startingTechs;
	};
}

#endif // STARTING_TECH_MAPPER_H
