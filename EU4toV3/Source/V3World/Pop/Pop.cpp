#include "Pop.h"

V2::Pop::Pop(std::string _type, int _size, std::string _culture, std::string _religion) :
	type(std::move(_type)), size(_size), culture(std::move(_culture)), religion(std::move(_religion)) {}

V2::Pop::Pop(const mappers::PopDetails& popDetails):
	 type(std::move(popDetails.popType)), size(popDetails.size), culture(std::move(popDetails.culture)), religion(std::move(popDetails.religion))
{
}

bool V2::Pop::combine(const Pop& rhs)
{
	if (culture == rhs.culture && religion == rhs.religion && type == rhs.type)
	{
		size += rhs.size;
		return true;
	}
	return false;
}
