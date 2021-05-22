#ifndef POP_H
#define POP_H

#include "../../Mappers/Pops/PopInstance.h"
#include <string>

namespace V2
{
class Pop
{
  public:
	Pop(std::string _type, int _size, std::string _culture, std::string _religion);
	Pop(const mappers::PopDetails& popDetails);

	bool combine(const Pop& rhs);
	void changeSize(int delta) { size += delta; }
	void incrementSupportedRegimentCount() { supportedRegiments++; }
	void blankCulture() { culture.clear(); }
	void blankReligion() { religion.clear(); }
	void setTrashed() { trashed = true; }

	int getSize() const { return size; }
	auto isTrashed() const { return trashed; }
	std::string getType() const { return type; }
	std::string getCulture() const { return culture; }
	std::string getReligion() const { return religion; }
	int getSupportedRegimentCount() const { return supportedRegiments; }
	bool isSlavePop() const { return type == "slaves" || culture.substr(0, 4) == "afro"; }

	friend std::ostream& operator<<(std::ostream& output, const Pop& pop);

  private:
	std::string type;
	int size = 0;
	std::string culture;
	std::string religion;
	int supportedRegiments = 0;
	bool trashed = false; // for combining
};
} // namespace V2

#endif // POP_H