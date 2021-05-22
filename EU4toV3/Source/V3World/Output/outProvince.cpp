#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Province& province)
{
	if (!province.details.owner.empty())
	{
		output << "owner=" << province.details.owner << "\n";
		output << "controller=" << province.details.controller << "\n";
	}
	for (const auto& core: province.details.cores)
	{
		output << "add_core=" << core << "\n";
	}
	if (!province.details.rgoType.empty())
	{
		output << "trade_goods = " << province.details.rgoType << "\n";
	}
	if (province.details.lifeRating > 0)
	{
		output << "life_rating = " << province.details.lifeRating << "\n";
	}
	if (!province.details.terrain.empty())
	{
		output << "terrain = " << province.details.terrain << "\n";
	}
	if (province.colonial > 0)
	{
		output << "colonial=" << province.colonial << "\n";
	}
	if (province.navalBaseLevel > 0)
	{
		output << "naval_base = " << province.navalBaseLevel << "\n";
	}
	if (province.fortLevel > 0)
	{
		output << "fort = " << province.fortLevel << "\n";
	}
	if (province.railLevel > 0)
	{
		output << "railroad = " << province.railLevel << "\n";
	}
	if (province.details.slaveState)
	{
		output << "is_slave = yes\n";
	}
	for (const auto& factory: province.factories)
	{
		output << *factory.second;
	}
	return output;
}

std::ostream& V2::operator<<(std::ostream& output, const std::optional<std::pair<int, std::vector<std::shared_ptr<Pop>>>>& pops)
{
	if (!pops) return output;
	if (!pops->first) return output;

	output << pops->first << " = {\n";
	for (const auto& pop : pops->second)
	{
		output << *pop;
	}
	output << "}\n";

	return output;
}