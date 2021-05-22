#ifndef ASSIMILATION_H
#define ASSIMILATION_H

#include "Parser.h"

namespace mappers
{
	class Assimilation : commonItems::parser
	{
	public:
		explicit Assimilation(std::istream& theStream);

		[[nodiscard]] auto getAssimilation() const { return assimilation; }
		
	private:
		int assimilation = 1;
	};
}

#endif // ASSIMILATION_H