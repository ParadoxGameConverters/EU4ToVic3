#ifndef CULTURE_DEF_H
#define CULTURE_DEF_H
#include "Color.h"

namespace mappers
{
struct CultureDef
{
	std::string name;
	std::optional<commonItems::Color> color;
	std::string religion;
	std::set<std::string> traits;
	std::set<std::string> maleCommonFirstNames;
	std::set<std::string> femaleCommonFirstNames;
	std::set<std::string> commonLastNames;
	std::set<std::string> nobleLastNames;
	std::set<std::string> maleRegalFirstNames;
	std::set<std::string> femaleRegalFirstNames;
	std::set<std::string> regalLastNames;
	std::set<std::string> ethnicities;
	std::string graphics;
	bool win1252Names = false;
	bool skipExport = false;

	std::map<std::string, std::string> locBlock;
};
} // namespace mappers

#endif // CULTURE_DEF_H
