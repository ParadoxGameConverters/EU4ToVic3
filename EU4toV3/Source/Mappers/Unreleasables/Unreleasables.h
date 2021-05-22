#ifndef UNRELEASABLES_H
#define UNRELEASABLES_H

#include "Parser.h"
#include <set>

namespace mappers
{
	class Unreleasables : commonItems::parser
	{
	public:
		Unreleasables();
		
		[[nodiscard]] bool isTagReleasable(const std::string& tag) const;

	private:
		std::set<std::string> unreleasables;
	};
}

#endif // UNRELEASABLES_H