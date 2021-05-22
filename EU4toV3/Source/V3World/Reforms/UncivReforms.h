#ifndef UNCIV_REFORMS_H
#define UNCIV_REFORMS_H

#include <ostream>

namespace V2
{	
	class Country;
	class UncivReforms {
	public:
		UncivReforms() = default;
		UncivReforms(int westernizationProgress, double milFocus, double socioEcoFocus, Country* country);

		friend std::ostream& operator<<(std::ostream& output, const UncivReforms& uncivReforms);

	private:
		bool reforms[16] = {};
	};
}


#endif // UNCIV_REFORMS_H