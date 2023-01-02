#include "MultiplyScript.h"

double V3::MultiplyScript::operator*(const double rhs) const
{
	return value * rhs;
}

V3::MultiplyScript V3::MultiplyScript::combine(const std::vector<MultiplyScript>& multiplyVector)
{
	MultiplyScript newMultiply{};

	for (const auto& mult: multiplyVector)
	{
		newMultiply.value += (1 - mult.value);	
	}
	newMultiply.value += 1;

	return newMultiply;
}
