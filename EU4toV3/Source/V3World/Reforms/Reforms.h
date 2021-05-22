#ifndef REFORMS_H
#define REFORMS_H

#include <memory>

namespace EU4
{
	class Country;
}

namespace V2
{	
	class CountryDetails;
	class Country;
	class Reforms
	{
	public:
		Reforms() = default;
		Reforms(const CountryDetails& details, const EU4::Country& srcCountry);
		
		friend std::ostream& operator<<(std::ostream& output, const Reforms& reforms);

	private:
		bool abolishSlavery = false;
		double slavery = 5.0;
		double vote_franchise = 5.0;
		double upper_house_composition = 5.0;
		double voting_system = 5.0;
		double public_meetings = 5.0;
		double press_rights = 5.0;
		double trade_unions = 5.0;
		double political_parties = 5.0;
	};
}

#endif // REFORMS_H