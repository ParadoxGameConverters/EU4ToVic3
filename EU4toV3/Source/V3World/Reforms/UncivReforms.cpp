#include "UncivReforms.h"
#include "../Country/Country.h"
#include "../Province/Province.h"

V2::UncivReforms::UncivReforms(const int westernizationProgress, const double milFocus, const double socioEcoFocus, Country* country)
{
	int westernizationCost[16];
	westernizationCost[0] = 15;
	westernizationCost[1] = 15;
	westernizationCost[2] = 15;
	westernizationCost[3] = 20;
	westernizationCost[4] = 15;
	westernizationCost[5] = 15;
	westernizationCost[6] = 20;
	westernizationCost[7] = 25;

	westernizationCost[8] = 10;
	westernizationCost[9] = 10;
	westernizationCost[10] = 10;
	westernizationCost[11] = 10;
	westernizationCost[12] = 10;
	westernizationCost[13] = 10;
	westernizationCost[14] = 10;
	westernizationCost[15] = 10;

	// Get all valid socio-economic reforms
	auto socioEconProgress = westernizationProgress * socioEcoFocus;
	for (unsigned int i = 0; i < 8; i++)
	{
		if (socioEconProgress >= westernizationCost[i] - 0.001)
		{
			reforms[i] = true;
			socioEconProgress -= westernizationCost[i];
		}
		else
		{
			reforms[i] = false;
		}
	}

	// Get all valid military reforms
	auto milProgress = westernizationProgress * milFocus;
	for (unsigned int i = 8; i < 16; i++)
	{
		if (milProgress >= westernizationCost[i] - 0.001)
		{
			reforms[i] = true;
			milProgress -= westernizationCost[i];
		}
		else
		{
			reforms[i] = false;
		}
	}

	// Use remaining progress to get any reforms in preferred category
	auto remainingProgress = socioEconProgress + milProgress;
	if (socioEconProgress >= milProgress)
	{
		for (unsigned int i = 0; i < 8; i++)
		{
			if (reforms[i]	== true)
			{
				continue;
			}
			if (remainingProgress >= westernizationCost[i] - 0.001)
			{
				reforms[i] = true;
				remainingProgress -= westernizationCost[i];
			}
		}
	}
	else
	{
		for (unsigned int i = 8; i < 16; i++)
		{
			if (reforms[i]	== true)
			{
				continue;
			}
			if (remainingProgress >= westernizationCost[i] - 0.001)
			{
				reforms[i] = true;
				remainingProgress -= westernizationCost[i];
			}
		}
	}

	// Use any remaining progress to get any possible reforms
	for (unsigned int i = 0; i < 16; i++)
	{
		if (reforms[i] == true)
		{
			continue;
		}
		if (remainingProgress >= westernizationCost[i] - 0.001)
		{
			reforms[i] = true;
			remainingProgress -= westernizationCost[i];
		}
	}

	// Implement special effects from reforms
	if (reforms[5] == true)
	{
		country->addRailroadtoCapitalState();
	}

	if (reforms[9] == true)
	{
		country->addTech("flintlock_rifles");
	}

	if (reforms[10] == true)
	{
		country->addTech("post_napoleonic_thought");
		auto provinces = country->getProvinces();
		auto provItr = provinces.find(country->getCapital());
		if (provItr != provinces.end())
		{
			provItr->second->setFortLevel(2);
		}
	}

	if (reforms[11] == true)
	{
		country->addTech("military_staff_system");
	}

	if (reforms[15] == true)
	{
		country->addTech("post_nelsonian_thought");
	}
}
