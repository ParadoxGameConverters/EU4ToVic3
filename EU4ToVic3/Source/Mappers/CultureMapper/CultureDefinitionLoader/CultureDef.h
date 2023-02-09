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
	bool skipProcessing = false; // We're skipping *separate* processing for defs that we import (from DW for example).
	bool skipExport = false;	  // We're skipping *separate* export for defs that we copy over from blankmod.

	std::map<std::string, std::string> locBlock;
	std::map<std::string, std::string> nameLocBlock; // all english since we load from disk
};
} // namespace mappers

#endif // CULTURE_DEF_H
