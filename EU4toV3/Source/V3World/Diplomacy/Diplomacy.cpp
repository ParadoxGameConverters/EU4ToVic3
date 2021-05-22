#include "Diplomacy.h"
#include "../../Configuration.h"
#include "../../EU4World/Country/EU4Country.h"
#include "../Country/Country.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "Relation.h"
#include <fstream>

void V2::Diplomacy::convertDiplomacy(std::vector<EU4::EU4Agreement> eu4agreements,
	 const mappers::CountryMappings& countryMapper,
	 std::map<std::string, std::shared_ptr<Country>>& countries)
{
	for (auto& agreement: eu4agreements)
	{
		auto EU4Tag1 = agreement.getOriginTag();
		auto ifV2Tag1 = countryMapper.getV2Tag(EU4Tag1);
		if (!ifV2Tag1)
			continue;
		auto V2Tag1 = *ifV2Tag1;

		auto EU4Tag2 = agreement.getTargetTag();
		auto ifV2Tag2 = countryMapper.getV2Tag(EU4Tag2);
		if (!ifV2Tag2)
			continue;
		auto V2Tag2 = *ifV2Tag2;

		const auto& country1 = countries.find(V2Tag1);
		const auto& country2 = countries.find(V2Tag2);
		if (country1 == countries.end())
		{
			LOG(LogLevel::Warning) << "Vic2 country " << V2Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (country2 == countries.end())
		{
			LOG(LogLevel::Warning) << "Vic2 country " << V2Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		// Stop creating relations for/with nations that didn't survive province conversion!
		if (country1->second->getProvinces().empty())
			continue;
		if (country2->second->getProvinces().empty())
			continue;

		auto& r1 = country1->second->getRelation(V2Tag2);
		auto& r2 = country2->second->getRelation(V2Tag1);

		if (agreementMapper.isAgreementInColonies(agreement.getAgreementType()))
		{
			std::map<Configuration::LIBERTYDESIRE, double> libertyMap = {
				 {Configuration::LIBERTYDESIRE::Loyal, 50.0},
				 {Configuration::LIBERTYDESIRE::Disloyal, 95.0},
				 {Configuration::LIBERTYDESIRE::Rebellious, 100.0},
			};

			// Do we annex or not?
			if (theConfiguration.getAbsorbColonies() == Configuration::ABSORBCOLONIES::AbsorbAll ||
				 theConfiguration.getAbsorbColonies() == Configuration::ABSORBCOLONIES::AbsorbSome &&
					  country2->second->getSourceCountry()->getLibertyDesire() < libertyMap[theConfiguration.getLibertyThreshold()])
			{
				LOG(LogLevel::Info) << " - " << country1->second->getTag() << " is absorbing " << country2->second->getTag() << " ("
										  << country2->second->getSourceCountry()->getLibertyDesire() << " vs " << libertyMap[theConfiguration.getLibertyThreshold()]
										  << " liberty desire)";
				country1->second->absorbColony(*country2->second);
				continue;
			}

			LOG(LogLevel::Info) << " - " << country1->second->getTag() << " is not absorbing " << country2->second->getTag() << " ("
									  << country2->second->getSourceCountry()->getLibertyDesire() << " vs " << libertyMap[theConfiguration.getLibertyThreshold()]
									  << " liberty desire)";
			country2->second->setColonyOverlord(V2Tag1);
		}

		if (agreementMapper.isAgreementInOnesiders(agreement.getAgreementType()))
		{
			processOnesider(r1);
		}

		if (agreementMapper.isAgreementInDoublesiders(agreement.getAgreementType()))
		{
			processDoublesider(r1, r2);
		}

		if (agreementMapper.isAgreementInTributaries(agreement.getAgreementType()))
		{
			processTributary(r1, r2);
		}

		if (agreementMapper.isAgreementInVassals(agreement.getAgreementType()))
		{
			// Yeah, we don't do marches, clients or all that. Or personal unions. PUs are a second relation
			// beside existing vassal relation specifying when vassalage ends.
			// However, vanilla Vic2 has PU end dates based on historical events, and we don't simulate those (for now).
			agreement.setAgreementType("vassal");
			processVassal(r1, r2);
			storeDevValues(*country1->second, *country2->second);
		}

		// In essence we should only recognize 3 diplomacy categories and these are it.
		if (agreement.getAgreementType() == "alliance" || agreement.getAgreementType() == "vassal" || agreement.getAgreementType() == "guarantee")
		{
			// copy agreement
			Agreement v2agreement(V2Tag1, V2Tag2, agreement.getAgreementType(), agreement.getStartDate());
			agreements.push_back(v2agreement);
		}
	}

	reduceVassalPrestige(countries);
	convertRelationsToInfluence(countries);
}

void V2::Diplomacy::storeDevValues(const Country& country1, const Country& country2)
{
	const auto& V2Tag1 = country1.getTag();
	const auto& V2Tag2 = country2.getTag();

	// We need to calculate weights and conglomerate weights (vassals + overlord) so that we can alter starting prestige
	// of individual vassals. We cannot do so yet as we don't know who's alive, dead or a vassal at all.
	if (!vassalCache.contains(V2Tag2))
		vassalCache[V2Tag2] = country2.getSourceCountry()->getCountryWeight();
	if (!masterCache.contains(V2Tag1))
		masterCache[V2Tag1] = country1.getSourceCountry()->getCountryWeight();
	masterVassals[V2Tag1].insert(V2Tag2);
}

void V2::Diplomacy::processOnesider(Relation& r1)
{
	// influence level +1, but never exceed 4
	// military access is not implied
	if (r1.getLevel() < 4)
		r1.setLevel(r1.getLevel() + 1);
	r1.increaseRelations(50);
}

void V2::Diplomacy::processDoublesider(Relation& r1, Relation& r2)
{
	// doublesiders are bidirectional; influence level +1, but never exceed 4
	// They don't set military access, as it's not implied (not even for alliances).
	if (r1.getLevel() < 4)
		r1.setLevel(r1.getLevel() + 1);
	r1.increaseRelations(50);
	if (r2.getLevel() < 4)
		r2.setLevel(r2.getLevel() + 1);
	r2.increaseRelations(50);
}

void V2::Diplomacy::processTributary(Relation& r1, Relation& r2)
{
	// influence level 5 - sphere, but not vassal, and military access is implied.
	r1.setLevel(5);
	r1.increaseRelations(75);
	r1.setInfluence(20);
	r1.setAccess(true);
	r2.setAccess(true);
}

void V2::Diplomacy::processVassal(Relation& r1, Relation& r2)
{
	r1.setLevel(5);
	r1.increaseRelations(75);
	r1.setInfluence(50);
	// In vic2 military access through vassals is not automatic but is implied.
	r1.setAccess(true);
	r2.setAccess(true);
}

void V2::Diplomacy::reduceVassalPrestige(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	// Alter prestige of vassals based on conglomerate weight size.
	for (const auto& conglomerate: masterVassals)
	{
		const auto& masterWeight = masterCache.at(conglomerate.first);
		auto conglomerateWeight = masterWeight;
		for (const auto& vassal: conglomerate.second)
			conglomerateWeight += vassalCache.at(vassal);
		if (conglomerateWeight == 0.0)
			continue;
		for (const auto& vassal: conglomerate.second)
		{
			const auto ratio = static_cast<double>(vassalCache.at(vassal)) / conglomerateWeight;
			const auto newPrestige = ratio * countries.find(vassal)->second->getPrestige();
			countries.find(vassal)->second->setPrestige(newPrestige);
		}
	}
}

void V2::Diplomacy::convertRelationsToInfluence(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	// Reward good starting relations with a small amount of extra influence, which will be relevant
	// to stating GPs, and will look natural.
	for (const auto& country: countries)
	{
		for (auto& relation: country.second->getRelations())
		{
			if (relation.second.getRelations() > 50)
			{
				const auto bonus = static_cast<int>((relation.second.getRelations() - 50) / 4);
				auto newInfluence = relation.second.getInfluence() + bonus;
				// Cash in excess influence for higher relationship level
				while (newInfluence >= 50)
				{
					// Just get to friendly, leave sphering to the player.
					if (relation.second.getLevel() < 4)
					{
						newInfluence -= 50;
						relation.second.setLevel(relation.second.getLevel() + 1);
					}
					else
					{
						break;
					}
				}
				relation.second.setInfluence(newInfluence);
			}
		}
	}
}

void V2::Diplomacy::sphereHRE(bool hreDecentralized,
	const std::shared_ptr<Country>& emperor,
	const std::map<std::string, std::shared_ptr<Country>>& countries
){
	if (!hreDecentralized || !emperor)
		return;

	Log(LogLevel::Info) << "\tSphereing HRE";
	for (const auto& country: countries)
	{
		if (!country.second->isMemberHRE() || country.second->isEmperorHRE())
			continue;
		auto& relation = emperor->getRelation(country.second->getTag());
		relation.setLevel(5);
		relation.setAccess(true);
	}
}

void V2::Diplomacy::output() const
{
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/diplomacy");

	std::ofstream alliances("output/" + theConfiguration.getOutputName() + "/history/diplomacy/Alliances.txt");
	if (!alliances.is_open())
		throw std::runtime_error("Could not create alliances history file!");

	std::ofstream guarantees("output/" + theConfiguration.getOutputName() + "/history/diplomacy/Guarantees.txt");
	if (!guarantees.is_open())
		throw std::runtime_error("Could not create guarantees history file!");

	std::ofstream puppetStates("output/" + theConfiguration.getOutputName() + "/history/diplomacy/PuppetStates.txt");
	if (!puppetStates.is_open())
		throw std::runtime_error("Could not create puppet states history file!");

	std::ofstream unions("output/" + theConfiguration.getOutputName() + "/history/diplomacy/Unions.txt");
	if (!unions.is_open())
		throw std::runtime_error("Could not create unions history file!");

	for (const auto& agreement: agreements)
	{
		if (agreement.getType() == "guarantee")
		{
			guarantees << agreement;
		}
		else if (agreement.getType() == "union")
		{
			unions << agreement;
		}
		else if (agreement.getType() == "vassal")
		{
			puppetStates << agreement;
		}
		else if (agreement.getType() == "alliance")
		{
			alliances << agreement;
		}
		else
		{
			LOG(LogLevel::Warning) << "Cannot output diplomatic agreement type " << agreement.getType() << "!";
		}
	}

	alliances.close();
	guarantees.close();
	puppetStates.close();
	unions.close();
}