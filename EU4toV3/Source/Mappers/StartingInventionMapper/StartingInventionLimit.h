#ifndef STARTING_INVENTION_LIMIT_H
#define STARTING_INVENTION_LIMIT_H

#include "Parser.h"

namespace mappers
{
	class StartingInventionLimit : commonItems::parser
	{
	public:
		explicit StartingInventionLimit(std::istream& theStream);
		
		[[nodiscard]] const auto& getTechName() const { return techName; }

	private:
		std::string techName;
	};
}

#endif // STARTING_INVENTION_LIMIT_H