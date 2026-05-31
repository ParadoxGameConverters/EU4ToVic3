#ifndef CULTURE_DEF_H
#define CULTURE_DEF_H
#include "Color.h"
#include <map>
#include <optional>
#include <set>
#include <string>

namespace mappers
{
enum class CultureDefinitionOperation
{
	Assign,
	Inject,
	Replace,
	TryInject,
	TryReplace,
	InjectOrCreate,
	ReplaceOrCreate
};

struct CultureDef
{
	CultureDefinitionOperation operation = CultureDefinitionOperation::Assign;
	std::string name;
	std::optional<commonItems::Color> color;
	bool hasColor = false;
	std::string religion;
	bool hasReligion = false;
	std::string language;
	bool hasLanguage = false;
	std::string heritage;
	bool hasHeritage = false;
	std::set<std::string> obsessions;
	std::set<std::string> traditions;
	std::set<std::string> maleCommonFirstNames;
	std::set<std::string> femaleCommonFirstNames;
	std::set<std::string> commonLastNames;
	std::set<std::string> nobleLastNames;
	std::set<std::string> maleRegalFirstNames;
	std::set<std::string> femaleRegalFirstNames;
	std::set<std::string> regalLastNames;
	std::set<std::string> ethnicities;
	std::string graphics;
	bool hasGraphics = false;
	bool skipProcessing = false; // We're skipping *separate* processing for defs that we import (from DW for example).
	bool skipExport = false;	  // We're skipping *separate* export for defs that we copy over from blankmod.

	std::map<std::string, std::string> locBlock;
	std::map<std::string, std::string> nameLocBlock; // all english since we load from disk
};
} // namespace mappers

#endif // CULTURE_DEF_H
