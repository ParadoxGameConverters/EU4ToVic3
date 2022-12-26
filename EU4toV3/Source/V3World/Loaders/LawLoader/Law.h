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
	int progressiveness = 0;
	std::set<std::string> requiredTechs;
	std::set<std::string> reqiredLaws;
	std::set<std::string> blockingLaws;
};
} // namespace V3
#endif // LAW_H