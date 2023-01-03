#ifndef AGREEMENT_H
#define AGREEMENT_H
#include "Date.h"

namespace V3
{
struct Agreement
{
	Agreement() = default;
	explicit Agreement(std::string _first, std::string _second, std::string _type, const date& _start_date):
		 type(std::move(_type)), first(std::move(_first)), second(std::move(_second)), start_date(_start_date)
	{
	}
	std::string type;
	std::string first;
	std::string second;
	date start_date;
	int duration = 0;
};
} // namespace V3

#endif // AGREEMENT_H