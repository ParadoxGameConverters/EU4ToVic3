#ifndef ASSIGNMENT_MAPPING_H
#define ASSIGNMENT_MAPPING_H
#include "Parser.h"

namespace mappers
{
class AssignmentMapping: commonItems::parser
{
  public:
	AssignmentMapping() = default;
	explicit AssignmentMapping(std::istream& theStream);

	[[nodiscard]] const auto& getAssignments() const { return assignments; }

  private:
	void registerKeys();

	std::map<std::string, std::string> assignments;
};
} // namespace mappers

#endif // ASSIGNMENT_MAPPING_H