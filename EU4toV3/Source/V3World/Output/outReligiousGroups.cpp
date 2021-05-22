#include "output.h"

std::ostream& mappers::operator<<(std::ostream& output, const ReligiousGroups& religiousGroupsMapper)
{
	for (const auto& [religiousGroupName, religiousGroup]: religiousGroupsMapper.getReligiousGroupsMap())
	{
		output << religiousGroupName << " = {\n";
		output << *religiousGroup;
		output << "}\n\n";
	}
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const ReligiousGroup& religiousGroup)
{
	for (const auto& [religionName, religion]: religiousGroup.religions)
	{
		output << "\t" << religionName << " = {\n";
		output << *religion;
		output << "\t}\n";
	}
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const Religion& religion)
{
	if (religion.icon > 0)
	{
		output << "\t\ticon = " << religion.icon << "\n";
	}
	if (!religion.colorString.empty())
		output << "\t\tcolor " << religion.colorString << "\n";
	return output;
}
