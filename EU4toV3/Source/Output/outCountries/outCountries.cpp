#include "outCountries.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
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
	for (const auto& element: country.getProcessedData().vanillaHistoryElements)
	{
		output << "\t\t" << element << "\n";
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
