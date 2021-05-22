#ifndef WAR_GOAL_MAPPER_H
#define WAR_GOAL_MAPPER_H

#include "Parser.h"
#include <map>
#include <set>
#include <optional>

namespace mappers
{

	class WarGoalMapper : commonItems::parser
	{
	public:
		WarGoalMapper();
		explicit WarGoalMapper(std::istream& theStream);
		
		[[nodiscard]] std::optional<std::string> translateWarGoal(const std::string& eu4WarGoal) const;

	private:
		void registerKeys();
		
		std::map<std::string, std::set<std::string>> warGoalMap;
	};
}

#endif // WAR_GOAL_MAPPER_H