#ifndef RELIGION_H
#define RELIGION_H
#include "Color.h"
#include <string>

namespace EU4
{
struct Religion
{
	Religion() = default;
	Religion(std::string name, std::string group = {}, std::string trappings = {}):
		 name(std::move(name)), group(std::move(group)), trappings(std::move(trappings))
	{
	}
	bool operator==(const Religion& rhs) const { return name == rhs.name && group == rhs.group && trappings == rhs.trappings; }

	std::string name;
	std::string group;
	std::string trappings; // a converted_dynamic_faith_107 can have "orthodox" trappings if split from orthodox in ck3 regardless of actual group.
	std::optional<commonItems::Color> color;
};
} // namespace EU4

#endif // RELIGION_H