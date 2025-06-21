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
	if (agreement.libertyDesire)
	{
		output << "\tc:" << agreement.second << " = {\n";
		output << "\t\tadd_liberty_desire = " << *agreement.libertyDesire << "\n";
		output << "\t}\n";
	}
}

void outCountryRelations(std::ostream& output, const std::string& tag, const std::map<std::string, V3::Relation>& relations)
{
	output << "\tc:" << tag << " = {\n";
	for (const auto& [target, relation]: relations)
		output << "\t\tset_relations = { country = c:" << target << " value = " << relation.getRelations() << " }\n";
	output << "\t}\n";
}

void outCountryRivals(std::ostream& output, const std::string& tag, const std::set<std::string>& rivals)
{
	output << "\tc:" << tag << " = {\n";
	for (const auto& rival: rivals)
	{
		output << "\t\tcreate_diplomatic_pact = {\n";
		output << "\t\t\tcountry = c:" << rival << "\n";
		output << "\t\t\ttype = rivalry\n";
		output << "\t\t}\n";
	}
	output << "\t}\n";
}

void outCountryTruces(std::ostream& output, const std::string& tag, const std::map<std::string, int>& truces)
{
	output << "\tc:" << tag << " ?= {\n";
	for (const auto& [target, duration]: truces)
	{
		output << "\t\tcreate_bidirectional_truce = {\n";
		output << "\t\t\tcountry = c:" << target << "\n";
		output << "\t\t\tmonths = " << duration << "\n";
		output << "\t\t}\n";
	}
	output << "\t}\n";
}

void outPowerBloc(std::ostream& output, const V3::PowerBloc& bloc)
{
	output << "\tc:" << bloc.owner << " = {\n";
	output << "\t\tcreate_power_bloc = {\n";
	output << "\t\t\tname = " << bloc.name << "\n";
	if (bloc.color)
		output << "\t\t\tmap_color " << *bloc.color << "\n";
	output << "\t\t\tfounding_date = " << bloc.start_date << "\n";
	output << "\t\t\tidentity = " << bloc.identity << "\n";
	output << "\t\t\tprinciple = " << bloc.principle << "\n";
	output << "\t\t\t\n";
	for (const auto& member: bloc.members)
	{
		output << "\t\t\tmember = c:" << member << "\n";
	}
	output << "\t\t}\n";
	output << "\t}\n\n";
}

} // namespace

void OUT::exportDiplomacy(const std::filesystem::path& outputName, const V3::PoliticalManager& politicalManager)
{
	exportPacts(outputName, politicalManager.getAgreements());
	exportRelations(outputName, politicalManager.getCountries());
	exportRivals(outputName, politicalManager.getCountries());
	exportTruces(outputName, politicalManager.getCountries());
	exportPowerBlocs(outputName, politicalManager.getPowerBlocs());
}

void OUT::exportPacts(const std::filesystem::path& outputName, const std::vector<V3::Agreement>& agreements)
{
	// std::ofstream defensivePacts("output" / outputName / "common/history/diplomacy/00_defensive_pacts.txt");
	// if (!defensivePacts.is_open())
	// 	throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_defensive_pacts.txt");

	std::ofstream subjects("output" / outputName / "common/history/diplomacy/00_subject_relationships.txt");
	if (!subjects.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_subject_relationships.txt");

	// std::ofstream trades("output" / outputName / "common/history/diplomacy/00_trade_agreement.txt");
	// if (!trades.is_open())
	//	throw std::runtime_error("Could not create " + outputName.string() + "/common/history/00_trade_agreement.txt");

	std::ofstream rivals("output" / outputName / "common/history/diplomacy/00_additional_rivalries.txt");
	if (!rivals.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_additional_rivalries.txt");

	// defensivePacts << commonItems::utf8BOM << "DIPLOMACY = {\n";
	subjects << commonItems::utf8BOM << "DIPLOMACY = {\n";
	// trades << commonItems::utf8BOM << "DIPLOMACY = {\n";
	rivals << commonItems::utf8BOM << "DIPLOMACY = {\n";

	for (const auto& agreement: agreements)
	{
		if (agreement.type == "defensive_pact")
			// outAgreement(defensivePacts, agreement);
			continue;
		else if (agreement.type == "trade_agreement")
			//	outAgreement(trades, agreement);
			continue;
		else if (agreement.type == "rivalry") // VN-imported rivalries are agreements, not country-bound rivalries, so they end up here.
			outAgreement(rivals, agreement);
		else
			outAgreement(subjects, agreement);
	}

	// defensivePacts << "}\n";
	subjects << "}\n";
	// trades << "}\n";
	rivals << "}\n";
	// defensivePacts.close();
	subjects.close();
	// trades.close();
	rivals.close();
}

void OUT::exportRelations(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream relations("output" / outputName / "common/history/diplomacy/00_relations.txt");
	if (!relations.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_relations.txt");

	relations << commonItems::utf8BOM << "DIPLOMACY = {\n";
	for (const auto& country: countries | std::views::values)
		if (!country->getRelations().empty())
			outCountryRelations(relations, country->getTag(), country->getRelations());

	relations << "}\n";
	relations.close();
}

void OUT::exportRivals(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream rivals("output" / outputName / "common/history/diplomacy/00_rivalries.txt");
	if (!rivals.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_rivalries.txt");

	rivals << commonItems::utf8BOM << "DIPLOMACY = {\n";
	for (const auto& country: countries | std::views::values)
		if (!country->getRivals().empty())
			outCountryRivals(rivals, country->getTag(), country->getRivals());

	rivals << "}\n";
	rivals.close();
}

void OUT::exportTruces(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream truces("output" / outputName / "common/history/diplomacy/00_truces.txt");
	if (!truces.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_truces.txt");

	truces << commonItems::utf8BOM << "DIPLOMACY = {\n";
	for (const auto& country: countries | std::views::values)
		if (!country->getTruces().empty())
			outCountryTruces(truces, country->getTag(), country->getTruces());

	truces << "}\n";
	truces.close();
}

void OUT::exportPowerBlocs(const std::filesystem::path& outputName, const std::vector<V3::PowerBloc>& powerBlocs)
{
	std::ofstream blocs("output" / outputName / "common/history/power_blocs/00_power_blocs.txt");
	if (!blocs.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/history/diplomacy/00_power_blocs.txt");

	blocs << commonItems::utf8BOM << "POWER_BLOCS = {\n";
	for (const auto& bloc: powerBlocs)
		outPowerBloc(blocs, bloc);

	blocs << "}\n";
	blocs.close();
}
