#include "outDiplomacy.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

namespace
{
void outAgreement(std::ostream& output, const V3::Agreement& agreement)
{
	output << "\tc:" << agreement.first << " = {\n";
	output << "\t\tcreate_diplomatic_pact = {\n";
	output << "\t\t\tcountry = c:" << agreement.second << "\n";
	output << "\t\t\ttype = " << agreement.type << "\n";
	output << "\t\t}\n";
	output << "\t}\n";
}

void outCountryRelations(std::ostream& output, const std::string& tag, const std::map<std::string, V3::Relation>& relations)
{
	output << "\tc:" << tag << " = {\n";
	for (const auto& [target, relation]: relations)
		output << "\t\tset_relations = { country = c:" << target << " value = " << relation.getRelations() << " }\n";
	output << "\t}\n";
}
} // namespace

void OUT::exportDiplomacy(const std::string& outputName, const V3::PoliticalManager& politicalManager)
{
	exportPacts(outputName, politicalManager.getAgreements());
	exportRelations(outputName, politicalManager.getCountries());
}

void OUT::exportPacts(const std::string& outputName, const std::vector<V3::Agreement>& agreements)
{
	std::ofstream defensivePacts("output/" + outputName + "/common/history/diplomacy/00_defensive_pacts.txt");
	if (!defensivePacts.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/diplomacy/00_defensive_pacts.txt");
	std::ofstream subjects("output/" + outputName + "/common/history/diplomacy/00_subject_relationships.txt");
	if (!subjects.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/diplomacy/00_subject_relationships.txt");

	defensivePacts << commonItems::utf8BOM << "DIPLOMACY = {\n";
	subjects << commonItems::utf8BOM << "DIPLOMACY = {\n";

	for (const auto& agreement: agreements)
	{
		if (agreement.type == "defensive_pact")
			outAgreement(defensivePacts, agreement);
		else
			outAgreement(subjects, agreement);
	}

	defensivePacts << "}\n";
	subjects << "}\n";
	defensivePacts.close();
	subjects.close();
}

void OUT::exportRelations(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream relations("output/" + outputName + "/common/history/diplomacy/00_relations.txt");
	if (!relations.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/diplomacy/00_relations.txt");

	relations << commonItems::utf8BOM << "DIPLOMACY = {\n";
	for (const auto& country: countries | std::views::values)
		if (!country->getRelations().empty())
			outCountryRelations(relations, country->getTag(), country->getRelations());

	relations << "}\n";
	relations.close();
}
