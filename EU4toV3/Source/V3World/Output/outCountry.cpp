#include "outCountry.h"
#include <ostream>

std::ostream& V3::operator<<(std::ostream& output, const Country& country)
{
	// TODO: TEST OUTPUT
	output << country.tag << " = {\n";
	if (country.processedData.color)
		output << "\tcolor " << *country.processedData.color << "\n";
	if (!country.processedData.type.empty())
		output << "\tcountry_type = " << country.processedData.type << "\n";
	if (!country.processedData.tier.empty())
		output << "\ttier = " << country.processedData.tier << "\n";
	if (!country.processedData.cultures.empty())
	{
		output << "\tcultures = { ";
		for (const auto& culture: country.processedData.cultures)
			output << culture << " ";
		output << "}\n";
	}
	if (!country.processedData.religion.empty())
		output << "\treligion = " << country.processedData.religion << "\n";
	if (!country.processedData.capitalStateName.empty())
		output << "\tcapital = " << country.processedData.capitalStateName << "\n";
	if (country.processedData.is_named_from_capital)
		output << "\tis_named_from_capital = true\n";

	output << "}\n\n";
	return output;
}
