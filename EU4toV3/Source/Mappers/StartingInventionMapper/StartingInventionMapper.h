#ifndef STARTING_INVENTION_MAPPER_H
#define STARTING_INVENTION_MAPPER_H

#include "Parser.h"
#include <map>
#include <set>

namespace mappers
{

	class StartingInventionMapper : commonItems::parser
	{
	public:
		StartingInventionMapper();
		explicit StartingInventionMapper(std::istream& theStream);
		
		[[nodiscard]] std::set<std::string> getInventionsForTechs(const std::set<std::string>& incTechs) const;

	private:
		void registerKeys();
		
		std::map<std::string, std::string> inventionTechMap;
	};
}

#endif // STARTING_INVENTION_MAPPER_H