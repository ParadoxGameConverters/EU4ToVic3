#ifndef SUPER_GROUP_MAPPER_H
#define SUPER_GROUP_MAPPER_H

#include "Parser.h"
#include <map>
#include <set>

namespace mappers
{
	class SuperGroupMapper : commonItems::parser
	{
	public:
		SuperGroupMapper() = default;
		
		void init();
		
		[[nodiscard]] std::optional<std::string> getGroupForSuperRegion(const std::string& superRegionName) const;
		[[nodiscard]] double getAssimilationFactor(const std::string& superRegionName) const;

	private:
		std::map<std::string, std::set<std::string>> superGroups; // name (old_world) - superregions in it
		std::map<std::string, int> assimilationMap; // superregion - assimilation rate
	};
}

#endif // SUPER_GROUP_MAPPER_H