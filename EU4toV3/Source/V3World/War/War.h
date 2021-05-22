#ifndef WAR_H
#define WAR_H

#include "../../EU4World/Wars/EU4War.h"
#include "../../Mappers/WarGoalMapper/WarGoalMapper.h"
#include "../../Mappers/ProvinceMappings/ProvinceMapper.h"
#include "../../Mappers/CountryMappings/CountryMappings.h"
#include "../Country/Country.h"
#include "../../EU4World/Wars/EU4WarDetails.h"

namespace V2
{
	class War
	{
	public:
		bool loadWar(const EU4::War& eu4War, 
			const mappers::WarGoalMapper& warGoalMapper,
			const mappers::ProvinceMapper& provinceMapper,
			const mappers::CountryMappings& countryMapper,
			std::map<std::string, std::shared_ptr<Country>>& countries);
		
		[[nodiscard]] std::string generateFileName() const;

		friend std::ostream& operator<<(std::ostream& output, const War& war);

	private:
		EU4::WarDetails details; // Reusing the class for storage. It was only slightly used anyways.
		std::string name;
		std::vector<std::string> attackers; // order maters! first is primary.
		std::vector<std::string> defenders; // order maters! first is primary.

		static std::optional<std::string> translateActor(
			const std::string& actor,
			const mappers::CountryMappings& countryMapper,
			std::map<std::string, std::shared_ptr<Country>>& countries);
	};
}

#endif // WAR_H