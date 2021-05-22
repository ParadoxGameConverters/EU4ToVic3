#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "../../Mappers/AgreementMapper/AgreementMapper.h"
#include "../../Mappers/CountryMappings/CountryMappings.h"
#include "../EU4toV3/Source/EU4World/Diplomacy/EU4Agreement.h"
#include "Agreement.h"
#include <vector>

namespace V2
{
class Relation;
class Country;
class Diplomacy
{
  public:
	void output() const;
	void addAgreement(const Agreement& agreement) { agreements.push_back(agreement); }
	void convertDiplomacy(std::vector<EU4::EU4Agreement> agreements,
		 const mappers::CountryMappings& countryMapper,
		 std::map<std::string, std::shared_ptr<Country>>& countries);
	void sphereHRE(bool hreDecentralized,
		const std::shared_ptr<Country>& emperor,
		const std::map<std::string, std::shared_ptr<Country>>& countries);

  private:
	std::vector<Agreement> agreements;

	mappers::AgreementMapper agreementMapper;
	std::map<std::string, double> conglomerateCache;
	std::map<std::string, double> vassalCache;
	std::map<std::string, double> masterCache;
	std::map<std::string, std::set<std::string>> masterVassals;

	static void convertRelationsToInfluence(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void reduceVassalPrestige(const std::map<std::string, std::shared_ptr<Country>>& countries);
	static void processVassal(Relation& r1, Relation& r2);
	static void processTributary(Relation& r1, Relation& r2);
	static void processOnesider(Relation& r1);
	static void processDoublesider(Relation& r1, Relation& r2);
	void storeDevValues(const Country& country1, const Country& country2);
};
} // namespace V2

#endif // DIPLOMACY_H