#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Country& country)
{
	if (country.details.capital > 0)
	{
		output << "capital = " << country.details.capital << "\n";
	}
	output << "primary_culture = " << country.details.primaryCulture << "\n";
	for (const auto& culture: country.details.acceptedCultures)
	{
		output << "culture = " << culture << "\n";
	}
	output << "religion = " << country.details.religion << "\n";
	output << "government = " << country.details.government << "\n";
	output << "plurality = " << country.details.plurality << "\n";
	output << "nationalvalue = " << country.details.nationalValue << "\n";
	output << "literacy = " << country.details.literacy << "\n";
	output << "\n";
	if (country.details.civilized)
	{
		output << "civilized = yes\n";
	}
	if (!country.details.isReleasableVassal)
	{
		output << "is_releasable_vassal = no\n";
	}
	output << "\n";
	output << "# Social Reforms\n";
	output << "wage_reform = no_minimum_wage\n";
	output << "work_hours = no_work_hour_limit\n";
	output << "safety_regulations = no_safety\n";
	output << "health_care = no_health_care\n";
	output << "unemployment_subsidies = no_subsidies\n";
	output << "pensions = no_pensions\n";
	output << "school_reforms = no_schools\n";

	if (country.reforms)
	{
		output << *country.reforms;
	}
	else
	{
		output << "# Political Reforms\n";
		output << "slavery=yes_slavery\n";
		output << "vote_franschise=none_voting\n"; // leave typo alone!
		output << "upper_house_composition=appointed\n";
		output << "voting_system=jefferson_method\n";
		output << "public_meetings=yes_meeting\n";
		output << "press_rights=censored_press\n";
		output << "trade_unions=no_trade_unions\n";
		output << "political_parties=underground_parties\n";
	}

	output << "\n";
	output << "ruling_party = " << country.details.rulingParty << "\n";
	output << "upper_house = \n";
	output << "{\n";
	output << "	fascist = 0\n";
	output << "	liberal = " << country.details.upperHouseLiberal << "\n";
	output << "	conservative = " << country.details.upperHouseConservative << "\n";
	output << "	reactionary = " << country.details.upperHouseReactionary << "\n";
	output << "	anarcho_liberal = 0\n";
	output << "	socialist = 0\n";
	output << "	communist = 0\n";
	output << "}\n";
	output << "\n";
	output << "# Starting Consciousness\n";
	output << "consciousness = " << country.details.consciousness << "\n";
	output << "nonstate_consciousness= " << country.details.nonstateConsciousness << "\n";
	output << "\n";
	output << "# Technologies\n";
	for (const auto& tech: country.techs)
	{
		output << tech << " = 1\n";
	}
	if (!country.details.civilized && country.uncivReforms)
	{
		output << *country.uncivReforms;
	}
	output << "prestige=" << country.details.prestige << "\n";

	if (!country.decisions.empty())
	{
		output << "\n";
		output << "# Decisions\n";
		output << "1.1.1 = {\n";
		for (const auto& decision: country.decisions)
		{
			output << "\tdecision = " << decision << "\n";
		}
		output << "}\n";
	}

	// output << "	schools=\"%s\"\n", techSchool.c_str());

	output << "oob = \"" << (country.tag + "_OOB.txt") << "\"\n";

	if (country.details.holyRomanEmperor)
	{
		output << "set_country_flag = emperor_hre\n";
	}
	else if (country.details.inHRE)
	{
		output << "set_country_flag = member_hre\n";
	}
	if (country.details.celestialEmperor)
	{
		output << "set_country_flag = celestial_emperor\n";
	}
	for (const auto& flag: country.details.countryFlags)
	{
		output << "set_country_flag = " << flag << "\n";
	}
	return output;
}

void V2::Country::outputCommons(std::ostream& output)
{
	output << "graphical_culture = ";
	if (!modCommons.getGraphicalCulture().empty())
		output << modCommons.getGraphicalCulture() << "\n";
	else if (!details.graphicalCulture.empty())
		output << details.graphicalCulture << "\n";
	else
		output << "UsGC\n"; // default to US graphics
	
	if (nationalColors.getMapColor())
		output << "color " << *nationalColors.getMapColor() << "\n";
	else if (!modCommons.getColorString().empty())
		output << "color " << modCommons.getColorString() << "\n";
	else if (!details.colorString.empty())
		output << "color " << details.colorString << "\n";

	if (!modCommons.getParties().empty())
	{
		for (const auto& party: modCommons.getParties())
			output << party;
	}
	else
	{
		for (const auto& party: details.parties)
			output << party;
	}

	if (!modCommons.getUnitNames().empty())
		output << "unit_names" << modCommons.getUnitNames();
	else if (!details.unitNames.empty())
		output << "unit_names" << details.unitNames;
}

void V2::Country::outputOOB(std::ostream& output)
{
	output << "#Sphere of Influence\n";
	output << "\n";
	for (const auto& relation: relations)
	{
		output << relation.second;
	}
	output << "\n";
	output << "#Leaders\n";
	for (const auto& leader: leaders)
	{
		output << leader;
	}
	output << "\n";
	output << "#Armies\n";
	for (const auto& army: armies)
	{
		output << army;
	}
}
