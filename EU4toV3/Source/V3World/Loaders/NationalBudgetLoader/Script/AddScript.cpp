#include "AddScript.h"

double V3::AddScript::operator+(const double rhs) const
{
	return value + rhs;
}

V3::AddScript V3::AddScript::combine(const std::vector<AddScript>& addVector)
{
	AddScript newAdd{};

	for (const auto& add: addVector)
	{
		newAdd.value += add.value;
	}

	return newAdd;
}