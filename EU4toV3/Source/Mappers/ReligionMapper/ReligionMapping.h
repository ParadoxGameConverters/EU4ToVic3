#ifndef RELIGION_MAPPING_H
#define RELIGION_MAPPING_H

#include "Parser.h"

namespace mappers
{
	class ReligionMapping: commonItems::parser
	{
	public:
		explicit ReligionMapping(std::istream& theStream);
		
		[[nodiscard]] const auto& getVic2Religion() const { return vic2Religion; }
		[[nodiscard]] const auto& getEU4Religions() const { return eu4Religions; }

	private:
		std::string vic2Religion;
		std::vector<std::string> eu4Religions;
	};
}

#endif // RELIGION_MAPPING_H