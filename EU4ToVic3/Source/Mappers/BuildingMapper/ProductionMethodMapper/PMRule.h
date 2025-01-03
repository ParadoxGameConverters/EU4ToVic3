#ifndef PM_RULE_H
#define PM_RULE_H
#include <string>

namespace mappers
{
struct PMRule
{
	std::string pm;
	double percent = 1.0;
	bool lawBound = false;

	bool operator==(const PMRule&) const = default;
};
} // namespace mappers
#endif // PM_RULE_H