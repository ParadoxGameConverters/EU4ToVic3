#ifndef V3_SOURCE_PROVINCE_DATA_H
#define V3_SOURCE_PROVINCE_DATA_H
#include "ProvinceManager/PopRatio.h"
#include <set>
#include <string>
#include <vector>

/* This data is transferred from eu4 provinces to v3 chunks, then passed along to v3 substates and then processed there. */

namespace EU4
{
class PopRatio;
}
namespace V3
{
struct SourceProvinceData
{
	std::string owner;
	double weight = 0;			  // absolute dev + buildings
	double investmentFactor = 0; // a popshaping factor that is a log function of investedWeight in the source province
	std::vector<EU4::PopRatio> popRatios;
	std::set<std::string> buildings;
	std::set<std::string> cores;
	bool territorialCore = false;
	bool sea = false;
	std::set<std::string> eu4Capitals;
};

} // namespace V3

#endif // V3_SOURCE_PROVINCE_DATA_H