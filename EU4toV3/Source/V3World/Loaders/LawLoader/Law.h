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
	int progressiveness = 0;			// used for sorting for "best" law.
	std::set<std::string> reqTechs;	// techs that are required for this law
	std::set<std::string> reqLaws;	// laws that are required for this law
	std::set<std::string> blockLaws; // laws that block this law
};
} // namespace V3
#endif // LAW_H