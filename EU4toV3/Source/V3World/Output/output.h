#ifndef OUTPUT_H
#define OUTPUT_H

#include "ModFile.h"
#include <ostream>

namespace V2
{
std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
} // namespace V2

namespace mappers
{
} // namespace mappers

#endif // OUTPUT_H