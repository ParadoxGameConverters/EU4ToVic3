#include "outDiplomacy.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

#include "Log.h"

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
} // namespace

void OUT::exportDiplomacy(const std::string& outputName, const V3::PoliticalManager& politicalManager)
{
	Log(LogLevel::Debug) << "Exporting diplo: " << politicalManager.getAgreements().size();
	std::ofstream defensivePacts("output/" + outputName + "/common/history/diplomacy/00_defensive_pacts.txt");
	if (!defensivePacts.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/diplomacy/00_defensive_pacts.txt");
	std::ofstream subjects("output/" + outputName + "/common/history/diplomacy/00_subject_relationships.txt");
	if (!subjects.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/diplomacy/00_subject_relationships.txt");

	defensivePacts << commonItems::utf8BOM << "DIPLOMACY = {\n";
	subjects << commonItems::utf8BOM << "DIPLOMACY = {\n";

	for (const auto& agreement: politicalManager.getAgreements())
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
