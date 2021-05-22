#ifndef TITLE_MAPPING_H
#define TITLE_MAPPING_H
#include "Parser.h"

namespace mappers
{
class TitleMapping: commonItems::parser
{
  public:
	explicit TitleMapping(std::istream& theStream);

	[[nodiscard]] bool hasIslamicRegion() const;
	[[nodiscard]] bool hasIndianRegion() const;

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getTitle() const { return title; }

  private:
	void registerKeys();
	
	std::string name;
	std::string title;
	std::string region;
};
} // namespace mappers

#endif // TITLE_MAPPING_H