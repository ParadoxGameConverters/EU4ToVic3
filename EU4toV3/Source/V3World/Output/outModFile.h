#ifndef OUTPUT_H
#define OUTPUT_H
#include <ostream>

namespace V3
{
class ModFile
{
  public:
	std::string outname;

	friend std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
};
std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
} // namespace V3

#endif // OUTPUT_H