#ifndef SUPERREGION_H
#define SUPERREGION_H

#include "Parser.h"
#include <set>

namespace mappers
{
	class SuperRegion : commonItems::parser
	{
	public:
		explicit SuperRegion(std::istream& theStream);

		[[nodiscard]] const auto& getSuperRegionAssimilationMap() const { return superRegionAssimilationMap; }
		[[nodiscard]] const auto& getSuperRegionNames() const { return superRegionNames; }
		
	private:
		std::map<std::string, int> superRegionAssimilationMap;
		std::set<std::string> superRegionNames;
	};
}

#endif // SUPERREGION_H