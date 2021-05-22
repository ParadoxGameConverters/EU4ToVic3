#ifndef STARTING_TECH_H
#define STARTING_TECH_H

#include "Parser.h"

namespace mappers
{
	class StartingTech : commonItems::parser
	{
	public:
		explicit StartingTech(std::istream& theStream);

		[[nodiscard]] const auto& getName() const { return name; }
		[[nodiscard]] const auto& getGroup() const { return group; }
		[[nodiscard]] auto getScore() const { return score; }
		
	private:
		std::string name;
		std::string hpm;
		std::string group;
		double score = -1;
	};
}

#endif // STARTING_TECH_H