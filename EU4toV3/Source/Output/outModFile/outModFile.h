#ifndef OUT_MODFILE_H
#define OUT_MODFILE_H
#include <ostream>

namespace OUT
{
void exportModFile(const std::string& outputName);
void outModFile(std::ostream& output, const std::string& outName);

} // namespace OUT

#endif // OUT_MODFILE_H