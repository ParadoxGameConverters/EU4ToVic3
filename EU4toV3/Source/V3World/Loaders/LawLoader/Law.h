#ifndef LAW_H
#define LAW_H
#include <set>
#include <string>

namespace V3
{
struct Law
{
	std::string name;
	std::string group;
	std::string institution;
	int progressiveness = 0;
	double bureaucracyCostMult = 0;
	std::set<std::string> requiredTechs;
	std::set<std::string> requiredLaws;
	std::set<std::string> blockingLaws;
	int barracksMax = 0;
};
} // namespace V3
#endif // LAW_H