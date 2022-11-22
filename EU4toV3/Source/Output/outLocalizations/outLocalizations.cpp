#include "outLocalizations.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

void OUT::exportCountryNamesAndAdjectives(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	const std::set<std::string> knownVic3Localizations =
		 {"braz_por", "english", "french", "german", "japanese", "korean", "polish", "russian", "simp_chinese", "spanish", "turkish"};

	for (const auto& language: knownVic3Localizations)
	{
		std::ofstream output("output/" + outputName + "/localization/" + language + "/converted_countries_l_" + language + ".yml");
		if (!output.is_open())
			throw std::runtime_error("output/" + outputName + "/localization/" + language + "/converted_countries_l_" + language + ".yml");

		output << commonItems::utf8BOM << "l_" << language << ":\n";
		for (const auto& country: countries | std::views::values)
		{
			output << " " << country->getTag() << ": \"" << country->getName(language) << "\"\n";
			output << " " << country->getTag() << "_ADJ: \"" << country->getAdjective(language) << "\"\n";
		}
		output.close();
	}
}
