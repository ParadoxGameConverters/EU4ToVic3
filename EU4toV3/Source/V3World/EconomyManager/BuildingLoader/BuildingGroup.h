#ifndef V3_BUILDING_GROUP_H
#define V3_BUILDING_GROUP_H
#include <string>

/* A Chunk is a collection of provinces defined by province_mappings.txt where a particular eu4 province maps to.
 * Its defining attribute is the single owner these provinces belong to. From Chunks we can generate substates, when
 * we overlay this political map over a geographical map.
 *
 * A chunk can be as small as a single V3 province, or as large as multiple V3 states.*/

namespace V3
{
struct BuildingGroup
{
	std::string name;
	std::string parent;
	int infrastructureCost = 0;
};
} // namespace V3

#endif // V3_BUILDING_GROUP_H
