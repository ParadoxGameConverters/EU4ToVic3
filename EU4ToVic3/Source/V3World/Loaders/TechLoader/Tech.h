#ifndef TECH_H
#define TECH_H

namespace V3
{
struct Tech
{
	int infrastructureMax = 0;
	double infrastructureMult = 0.0;
	int throughputMax = 0;
	std::map<std::string, int> maxBuildingLevels;
};
} // namespace V3
#endif // LAW_H