#ifndef OUT_CONVERTER_VERSION
#define OUT_CONVERTER_VERSION

#include "ConverterVersion/ConverterVersion.h"

namespace mappers
{
std::ostream& operator<<(std::ostream& output, const ConverterVersion& version);
}



#endif // OUT_CONVERTER_VERSION