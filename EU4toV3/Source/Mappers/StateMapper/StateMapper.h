#ifndef STATE_MAPPER_H
#define STATE_MAPPER_H

#include "Parser.h"
#include <map>
#include <set>

namespace mappers
{
	class StateMapper: commonItems::parser
	{
	public:
		StateMapper();
		explicit StateMapper(std::istream& theStream);
		
		[[nodiscard]] std::set<int> getAllProvincesInState(int province) const;

	private:
		void registerKeys();
		std::map<int, std::set<int>> stateProvincesMap;
	};
}

#endif // STATE_MAPPER_H