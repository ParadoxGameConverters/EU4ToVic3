#ifndef RELIGION_MAPPING_H
#define RELIGION_MAPPING_H
#include "Parser.h"

namespace mappers
{
class ReligionMapping: commonItems::parser
{
  public:
	explicit ReligionMapping(std::istream& theStream);

	[[nodiscard]] const auto& getV3Religion() const { return v3Religion; }
	[[nodiscard]] const auto& getEU4Religions() const { return eu4Religions; }

  private:
	void registerkeys();

	std::string v3Religion;
	std::set<std::string> eu4Religions;
};
} // namespace mappers

#endif // RELIGION_MAPPING_H