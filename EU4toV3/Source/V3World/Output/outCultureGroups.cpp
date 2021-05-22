#include "CultureGroups/CultureGroup.h"
#include "output.h"

std::ostream& mappers::operator<<(std::ostream& output, const CultureGroups& cultureGroupsMapper)
{
	for (const auto& [cultureGroupName, cultureGroup]: cultureGroupsMapper.getCultureGroupsMap())
	{
		output << cultureGroupName << " = {\n";
		output << *cultureGroup;
		output << "}\n\n";
	}
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const CultureGroup& cultureGroup)
{
	output << "\tleader = " << cultureGroup.leader << "\n";
	if (!cultureGroup.isOverseas.empty())
	{
		output << "\tis_overseas = " << cultureGroup.isOverseas << "\n";
	}
	output << "\tunit = " << cultureGroup.unit << "\n";
	output << "\t\n";
	for (const auto& [cultureName, culture]: cultureGroup.cultures)
	{
		output << "\t" << cultureName << " = {\n";
		output << *culture;
		output << "\t}\n";
	}
	output << "\t\n";
	if (!cultureGroup.culturalUnionTag.empty())
	{
		output << "\tunion = " << cultureGroup.culturalUnionTag << "\n";
		output << "\t\n";
	}
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const Culture& culture)
{
	if (culture.color)
		output << "\t\tcolor " << *culture.color << "\n";
	if (culture.radicalism)
	{
		output << "\t\tradicalism = " << culture.radicalism << "\n";
	}
	if (!culture.primaryTag.empty())
	{
		output << "\t\tprimary = " << culture.primaryTag << "\n";
	}
	output << "\t\t\n";
	output << "\t\tfirst_names = { ";
	for (const auto& firstName: culture.firstNames)
	{
		output << firstName << " ";
	}
	output << "}\n";
	output << "\t\t\n";
	output << "\t\tlast_names = { ";
	for (const auto& lastName: culture.lastNames)
	{
		output << lastName << " ";
	}
	output << "}\n";
	output << "\t\t\n";
	return output;
}
