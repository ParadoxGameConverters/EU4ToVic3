#ifndef STARTING_INVENTION_DETAILS_H
#define STARTING_INVENTION_DETAILS_H

#include "Parser.h"

namespace mappers
{
	class StartingInventionDetails : commonItems::parser
	{
	public:
		explicit StartingInventionDetails(std::istream& theStream);
		
		[[nodiscard]] const auto& getTechName() const { return techName; }

	private:
		std::string techName;		
	};
}

#endif // STARTING_INVENTION_DETAILS_H