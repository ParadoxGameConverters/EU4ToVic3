#ifndef OUT_METADATA_FILE_H
#define OUT_METADATA_FILE_H
#include <ostream>

namespace OUT
{
void exportMetadataFile(const std::string& outputName);
void outMetadataFile(std::ostream& output, const std::string& outName);

} // namespace OUT

#endif // OUT_METADATA_FILE_H