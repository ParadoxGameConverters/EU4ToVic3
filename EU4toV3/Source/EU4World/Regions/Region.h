#ifndef EU4_REGION_H
#define EU4_REGION_H
#include "Parser.h"
#include "Areas.h"
#include <set>

namespace EU4
{
	class Region: commonItems::parser
	{
	public:
		explicit Region(std::istream& theStream);
		explicit Region(const std::set<int>& provinces);
		
		[[nodiscard]] bool regionContainsProvince(int province) const;
		[[nodiscard]] bool areaContainsProvince(const std::string& areaName, int province) const;		
		[[nodiscard]] const auto& getAreaNames() const { return areaNames; }

		void addProvinces(const Areas& areas);

	private:
		void registerKeys();
		
		std::map<std::string, std::set<int>> areaProvinces;
		std::set<std::string> areaNames;
	};
}

#endif // EU4_REGION_H