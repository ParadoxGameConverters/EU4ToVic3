#include "outCountries.h"
#include "ClayManager/State/SubState.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
void outMajorFormable(std::ostream& output, const std::string& tag, const V3::MajorFormablesEntry& formable)
{
	output << tag << " = {\n";
	for (const auto& stanza: formable.getStanzas())
		output << "\t" << stanza << "\n";
	output << "\tpossible = {\n";
	output << "\t\tOR = {\n";
	for (const auto& culture: formable.getCultures())
		output << "\t\t\tcountry_has_primary_culture = cu:" << culture << "\n";
	for (const auto& possibleStanza: formable.getPossibleStanzas())
		output << "\t\t" << possibleStanza << "\n";
	output << "\t\t}\n";
	output << "\t\tany_country = {\n";
	output << "\t\t\tOR = {\n";
	for (const auto& culture: formable.getCultures())
		output << "\t\t\t\tcountry_has_primary_culture = cu:" << culture << "\n";
	output << "\t\t\t}\n";
	output << "\t\t\thas_technology_researched = " << formable.getRequiredTechnology() << "\n";
	output << "\t\t}\n";
	output << "\t}\n";
	output << "}\n\n";
}

void outReleasableCountry(std::ostream& output, const V3::Country& country)
{
	output << country.getTag() << " = {\n";
	output << "\tprovinces = { ";
	for (const auto& province: country.getUnownedProvinces())
		output << province << " ";
	output << " }\n";
	output << "\tai_will_do = { always = no }\n";
	output << "}\n\n";
}

void outCommonCountry(std::ostream& output, const V3::Country& country)
{
	output << country.getTag() << " = {\n";

	if (country.getProcessedData().color)
		output << "\tcolor " << *country.getProcessedData().color << "\n";
	if (!country.getProcessedData().type.empty())
		output << "\tcountry_type = " << country.getProcessedData().type << "\n";
	if (!country.getProcessedData().tier.empty())
		output << "\ttier = " << country.getProcessedData().tier << "\n";
	if (!country.getProcessedData().cultures.empty())
	{
		output << "\tcultures = { ";
		for (const auto& culture: country.getProcessedData().cultures)
			output << culture << " ";
		output << "}\n";
	}
	if (!country.getProcessedData().religion.empty())
		output << "\treligion = " << country.getProcessedData().religion << "\n";
	if (!country.getProcessedData().capitalStateName.empty())
		output << "\tcapital = " << country.getProcessedData().capitalStateName << "\n";
	if (country.getProcessedData().is_named_from_capital)
		output << "\tis_named_from_capital = yes\n";

	output << "}\n\n";
}

void outHistoryCountry(std::ostream& output, const V3::Country& country)
{
	output << "\tc:" << country.getTag() << " = {\n";
	for (const auto& tech: country.getProcessedData().techs)
		output << "\t\tadd_technology_researched = " << tech << "\n";
	for (const auto& effect: country.getProcessedData().effects)
		output << "\t\t" << effect << " = yes\n";
	for (const auto& law: country.getProcessedData().laws)
		output << "\t\tactivate_law = law_type:" << law << "\n";
	if (!country.getProcessedData().ideaEffect.rulingInterestGroups.empty())
	{
		output << "\t\tset_ruling_interest_groups = {\n\t\t\t";
		for (const auto& ig: country.getProcessedData().ideaEffect.rulingInterestGroups)
			output << ig << " ";
		output << "\n\t\t}\n";
	}
	for (const auto& ig: country.getProcessedData().ideaEffect.boostedInterestGroups)
	{
		output << "\t\tig:" << ig << " = {\n";
		output << "\t\t\tset_ig_bolstering = yes\n ";
		output << "\t\t}\n";
	}
	for (const auto& ig: country.getProcessedData().ideaEffect.suppressedInterestGroups)
	{
		output << "\t\tig:" << ig << " = {\n";
		output << "\t\t\tset_ig_suppression = yes\n";
		output << "\t\t}\n";
	}
	for (const auto& [ig, modifier]: country.getProcessedData().igIdeologyModifiers)
	{
		if (modifier.addedIdeologies.empty() && modifier.removedIdeologies.empty())
			continue;
		output << "\t\tig:" << ig << " = {\n";
		for (const auto& ideology: modifier.removedIdeologies)
			output << "\t\t\tremove_ideology = " << ideology << "\n ";
		for (const auto& ideology: modifier.addedIdeologies)
			output << "\t\t\tadd_ideology = " << ideology << "\n ";
		output << "\t\t}\n";
	}
	for (const auto& element: country.getProcessedData().vanillaHistoryElements)
	{
		output << "\t\t" << element << "\n";
	}
	for (const auto& [institution, level]: country.getProcessedData().institutions)
	{
		output << "\t\tset_institution_investment_level = {\n";
		output << "\t\t\tinstitution = " << institution << "\n";
		output << "\t\t\tlevel = " << level << "\n";
		output << "\t\t}\n";
	}
	output << "\t}\n";
}

void outHistoryPopulations(std::ostream& output, const V3::Country& country)
{
	output << "\tc:" << country.getTag() << " = {\n";
	for (const auto& effect: country.getProcessedData().populationEffects)
		output << "\t\t" << effect << " = yes\n";
	for (const auto& element: country.getProcessedData().vanillaPopulationElements)
		output << "\t\t" << element << "\n";
	output << "\t}\n";
}

} // namespace

void OUT::exportCommonCountries(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/country_definitions/99_converted_countries.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/country_definitions/99_converted_countries.txt");

	output << commonItems::utf8BOM;
	for (const auto& country: countries | std::views::values)
		outCommonCountry(output, *country);
	output.close();
}

void OUT::exportHistoryCountries(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/history/countries/99_converted_countries.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/countries/99_converted_countries.txt");

	output << commonItems::utf8BOM << "COUNTRIES = {\n";
	for (const auto& country: countries | std::views::values)
		if (!country->getSubStates().empty())
			outHistoryCountry(output, *country);
	output << "}\n";
	output.close();
}

void OUT::exportHistoryPopulations(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/history/population/99_converted_countries.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/population/99_converted_countries.txt");

	output << commonItems::utf8BOM << "POPULATION = {\n";
	for (const auto& country: countries | std::views::values)
		if (!country->getSubStates().empty() &&
			 (!country->getProcessedData().populationEffects.empty() || !country->getProcessedData().vanillaPopulationElements.empty()))
			outHistoryPopulations(output, *country);
	output << "}\n";
	output.close();
}

void OUT::exportReleasables(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/country_creation/99_converted_releasables.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/country_creation/99_converted_releasables.txt");

	output << commonItems::utf8BOM;
	for (const auto& country: countries | std::views::values)
		if (country->getSubStates().empty() && !country->getUnownedProvinces().empty())
			outReleasableCountry(output, *country);
	output.close();
}

void OUT::exportMajorFormables(const std::string& outputName, const std::map<std::string, V3::MajorFormablesEntry>& formables)
{
	std::ofstream output("output/" + outputName + "/common/country_formation/00_major_formables.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/country_formation/00_major_formables.txt");

	output << commonItems::utf8BOM;
	for (const auto& [tag, formable]: formables)
		outMajorFormable(output, tag, formable);
	output.close();
}
