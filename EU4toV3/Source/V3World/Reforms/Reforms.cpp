#include "Reforms.h"
#include "../Country/Country.h"

V2::Reforms::Reforms(const CountryDetails& details, const EU4::Country& srcCountry):
	slavery(details.reforms.slavery),
	vote_franchise(details.reforms.vote_franchise),
	upper_house_composition(details.reforms.upper_house_composition),
	voting_system(details.reforms.voting_system),
	public_meetings(details.reforms.public_meetings),
	press_rights(details.reforms.press_rights),
	trade_unions(details.reforms.trade_unions),
	political_parties(details.reforms.political_parties)
{
	if (srcCountry.hasModifier("the_abolish_slavery_act") || srcCountry.hasModifier("abolish_slavery_act"))
	{
		abolishSlavery = true;
	}
}
