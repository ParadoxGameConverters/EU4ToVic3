#ifndef RELIGION_DEF_H
#define RELIGION_DEF_H
#include "Color.h"
#include <string>

namespace mappers
{
struct ReligionDef
{
	std::string name;
	std::string texture;
	std::set<std::string> traits;
	std::optional<commonItems::Color> color;
	std::set<std::string> taboos;

	std::map<std::string, std::string> locBlock;
};
} // namespace mappers

#endif // RELIGION_DEF_H
