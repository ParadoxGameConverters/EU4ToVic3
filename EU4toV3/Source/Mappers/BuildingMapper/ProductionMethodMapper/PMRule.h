#ifndef PM_RULE_H
#define PM_RULE_H
#include <string>

namespace mappers
{
struct PMRule
{
	std::string name;
	double percent = 1.0;

	bool operator==(const PMRule& rhs) const { return name == rhs.name && percent == rhs.percent; }
};
} // namespace mappers
#endif // PM_RULE_H