#include "outCharacters.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

namespace
{
void outCharacter(std::ostream& output, const V3::Character& character)
{
	output << "\t\tcreate_character = {\n";
	if (!character.firstName.empty())
		output << "\t\t\tfirst_name = \"" << character.firstName << "\"\n";
	if (!character.lastName.empty())
		output << "\t\t\tlast_name = \"" << character.lastName << "\"\n";
	if (character.ruler)
		output << "\t\t\truler = yes\n";
	if (character.heir)
		output << "\t\t\their = yes\n";
	if (character.general)
		output << "\t\t\tis_general = yes\n";
	if (character.admiral)
		output << "\t\t\tis_admiral = yes\n";
	if (!character.commanderRank.empty())
		output << "\t\t\tcommander_rank = " << character.commanderRank << "\n";
	if (character.female)
		output << "\t\t\tfemale = yes\n";
	if (!character.interestGroup.empty())
		output << "\t\t\tinterest_group = " << character.interestGroup << "\n";
	if (!character.culture.empty())
		output << "\t\t\tculture = cu:" << character.culture << "\n";
	if (!character.religion.empty())
		output << "\t\t\treligion = rel:" << character.religion << "\n";
	if (character.birthDate.isSet())
		output << "\t\t\tbirth_date = " << character.birthDate << "\n";
	if (!character.traits.empty())
	{
		output << "\t\t\ttraits = {\n\t\t\t\t";
		for (const auto& trait: character.traits)
			output << trait << " ";
		output << "\n\t\t\t}\n";
	}
	if (character.married)
	{
		output << "\t\t\ton_created = {\n";
		output << "\t\t\t\tset_variable = is_married\n";
		output << "\t\t\t}\n";
	}
	output << "\t\t}\n\n";
}
} // namespace

void OUT::exportCharacters(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	for (const auto& [tag, country]: countries)
	{
		if (country->getSubStates().empty())
			continue;
		if (country->getProcessedData().characters.empty() && country->getProcessedData().vanillaCharacterElements.empty())
			continue;

		std::ofstream output("output/" + outputName + "/common/history/characters/00_" + tag + ".txt");
		if (!output.is_open())
			throw std::runtime_error("Could not create " + outputName + "/common/history/characters/00_" + tag + ".txt");

		output << commonItems::utf8BOM << "CHARACTERS = {\n";
		output << "\tc:" << tag << " = {\n";
		for (const auto& character: country->getProcessedData().characters)
		{
			outCharacter(output, character);
		}
		for (const auto& element: country->getProcessedData().vanillaCharacterElements)
		{
			output << "\t\t" << element << "\n";
		}
		output << "\t}\n";
		output << "}\n";
		output.close();
	}
}
