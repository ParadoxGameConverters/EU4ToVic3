#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Localisation& localisation)
{
	output << Localisation::convert(localisation.tag);
	for (int i = 0; i < V2::Localisation::numLanguages; ++i)
	{
		output << ';' << Localisation::convert(localisation.name[i], i);
	}
	output << "x\n";

	output << Localisation::convert(localisation.tag) << "_ADJ";
	for (int i = 0; i < V2::Localisation::numLanguages; ++i)
	{
		output << ';' << Localisation::convert(localisation.adjective[i], i);
	}
	output << "x\n";

	for (const auto& party: localisation.parties)
	{
		output << Localisation::convert(party.key);
		for (int i = 0; i < V2::Localisation::numLanguages; ++i)
		{
			output << ';' << Localisation::convert(party.name[i], i);
		}
		output << "x\n";
	}

	return output;
}
