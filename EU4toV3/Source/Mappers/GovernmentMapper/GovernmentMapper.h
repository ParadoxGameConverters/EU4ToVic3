#ifndef GOVERNMENT_MAPPER_H
#define GOVERNMENT_MAPPER_H
#include "Parser.h"

namespace mappers
{
class GovernmentMapper: commonItems::parser
{
  public:
	GovernmentMapper();
	explicit GovernmentMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> matchGovernment(const std::string& sourceGovernment) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> governmentMap;
};
}; // namespace mappers

#endif // GOVERNMENT_MAPPER_H