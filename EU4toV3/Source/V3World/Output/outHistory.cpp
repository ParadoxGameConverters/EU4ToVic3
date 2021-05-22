#include "../V3World.h"

std::ostream& V2::operator<<(std::ostream& output, const std::vector<std::pair<std::string, EU4::HistoricalEntry>>& historicalData)
{
	for (const auto& entry: historicalData)
	{
		output << entry.first << " = {\n";
		if (entry.second.monarchy)
		{
			output << "\tmonarchy = yes\n";
		}
		else
		{
			output << "\tmonarchy = no\n";
		}
		if (!entry.second.lastDynasty.empty())
		{
			output << "\tlast_dynasty = \"" << entry.second.lastDynasty << "\"\n";
		}
		output << "}\n";
	}
	return output;
}
