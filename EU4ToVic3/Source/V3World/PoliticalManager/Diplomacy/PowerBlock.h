#ifndef POWERBLOCK_H
#define POWERBLOCK_H
#include "Date.h"

namespace V3
{
struct PowerBlock
{
	PowerBlock() = default;
	explicit PowerBlock(std::string _owner, std::set<std::string> _members, std::string _type, const date& _start_date):
		 type(std::move(_type)), owner(std::move(_owner)), members(std::move(_members)), start_date(_start_date)
	{
	}
	std::string type;
	std::string owner;
	std::set<std::string> members;
	date start_date;
};
} // namespace V3

#endif // POWERBLOCK_H