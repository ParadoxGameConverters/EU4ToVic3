#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Reforms& reforms)
{
	output << "\n";
	output << "# political reforms\n";
	if (reforms.abolishSlavery || reforms.slavery > 0)
	{
		output << "slavery=no_slavery\n";
	}
	else
	{
		output << "slavery=yes_slavery\n";
	}

	// Don't correct PDX typo!
	if (reforms.vote_franchise > 4)
	{
		output << "vote_franschise=universal_voting\n";
	}
	else if (reforms.vote_franchise > 2.5)
	{
		output << "vote_franschise=universal_weighted_voting\n";
	}
	else if (reforms.vote_franchise > 0.5)
	{
		output << "vote_franschise=wealth_voting\n";
	}
	else if (reforms.vote_franchise > -1.5)
	{
		output << "vote_franschise=wealth_weighted_voting\n";
	}
	else if (reforms.vote_franchise > -2.5)
	{
		output << "vote_franschise=landed_voting\n";
	}
	else
	{
		output << "vote_franschise=none_voting\n";
	}

	if (reforms.upper_house_composition > 2.5)
	{
		output << "upper_house_composition=population_equal_weight\n";
	}
	else if (reforms.upper_house_composition > 0)
	{
		output << "upper_house_composition=state_equal_weight\n";
	}
	else if (reforms.upper_house_composition > -2.5)
	{
		output << "upper_house_composition=appointed\n";
	}
	else
	{
		output << "upper_house_composition=party_appointed\n";
	}

	if (reforms.voting_system > 2.5)
	{
		output << "voting_system=proportional_representation\n";
	}
	else if (reforms.voting_system > -2)
	{
		output << "voting_system=jefferson_method\n";
	}
	else
	{
		output << "voting_system=first_past_the_post\n";
	}

	if (reforms.public_meetings > 0)
	{
		output << "public_meetings=yes_meeting\n";
	}
	else
	{
		output << "public_meetings=no_meeting\n";
	}

	if (reforms.press_rights > 2.5)
	{
		output << "press_rights=free_press\n";
	}
	else if (reforms.press_rights > -1.5)
	{
		output << "press_rights=censored_press\n";
	}
	else
	{
		output << "press_rights=state_press\n";
	}

	if (reforms.trade_unions > 2.5)
	{
		output << "trade_unions=all_trade_unions\n";
	}
	else if (reforms.trade_unions > 0)
	{
		output << "trade_unions=non_socialist\n";
	}
	else if (reforms.trade_unions > -2.5)
	{
		output << "trade_unions=state_controlled\n";
	}
	else
	{
		output << "trade_unions=no_trade_unions\n";
	}

	if (reforms.political_parties > 3)
	{
		output << "political_parties=secret_ballots\n";
	}
	else if (reforms.political_parties > 1)
	{
		output << "political_parties=non_secret_ballots\n";
	}
	else if (reforms.political_parties > -1)
	{
		output << "political_parties=gerrymandering\n";
	}
	else if (reforms.political_parties > -3)
	{
		output << "political_parties=harassment\n";
	}
	else
	{
		output << "political_parties=underground_parties\n";
	}
	return output;
}
