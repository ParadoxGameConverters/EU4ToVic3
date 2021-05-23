#ifndef CONVERTER_VERSION_H
#define CONVERTER_VERSION_H

#include "Parser.h"

namespace mappers
{
	class ConverterVersion : commonItems::parser
	{
	public:
		ConverterVersion();
	 explicit ConverterVersion(std::istream& theStream);

		friend std::ostream& operator<<(std::ostream& output, const ConverterVersion& versionParser);

	private:
		void registerKeys();

		std::string name;
		std::string version;
		std::string descriptionLine;
	};
}

#endif // CONVERTER_VERSION_H