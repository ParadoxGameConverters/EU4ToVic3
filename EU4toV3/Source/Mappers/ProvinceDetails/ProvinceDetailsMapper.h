#ifndef PROVINCE_DETAILS_MAPPER_H
#define PROVINCE_DETAILS_MAPPER_H
#include "Parser.h"
#include <set>

namespace mappers
{
typedef struct ProvinceDetails
{
	// This is a storage container for V2::Province.
	bool slaveState = false;
	int lifeRating = 0;
	int colonial = 0;
	int colonyLevel = 0;
	int navalBaseLevel = 0;
	int fortLevel = 0;
	int railLevel = 0;
	std::string owner;
	std::string controller;
	std::string rgoType;
	std::string terrain;
	std::string climate;
	std::set<std::string> cores;
} ProvinceDetails;

class ProvinceDetailsMapper: commonItems::parser
{
  public:
	ProvinceDetailsMapper() = default;
	explicit ProvinceDetailsMapper(const std::string& filename);
	explicit ProvinceDetailsMapper(std::istream& theStream);

	[[nodiscard]] const auto& getProvinceDetails() const { return provinceDetails; }

  private:
	void registerKeys();

	ProvinceDetails provinceDetails;
};
} // namespace mappers

#endif // PROVINCE_DETAILS_MAPPER_H
