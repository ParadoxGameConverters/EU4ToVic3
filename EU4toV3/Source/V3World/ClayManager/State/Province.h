#ifndef V3_PROVINCE_H
#define V3_PROVINCE_H
#include <string>

namespace V3
{
class Chunk;
class Province
{
  public:
	Province() = default;

	[[nodiscard]] auto getName() const { return name; }
	[[nodiscard]] auto getTerrain() const { return terrain; }
	[[nodiscard]] auto isSea() const { return sea; }
	[[nodiscard]] auto isLake() const { return lake; }
	[[nodiscard]] auto isPrime() const { return prime; }
	[[nodiscard]] auto isImpassable() const { return impassable; }

	void setSea() { sea = true; }
	void setLake()
	{
		lake = true;
		impassable = true;
	}
	void setPrime() { prime = true; }
	void setImpassable() { impassable = true; }
	void setName(const std::string& theName) { name = theName; }
	void setTerrain(const std::string& theTerrain) { terrain = theTerrain; };

  private:
	void registerKeys();

	std::string name;
	std::string terrain;
	bool sea = false;
	bool lake = false;
	bool prime = false;
	bool impassable = false;
};
} // namespace V3

#endif // V3_PROVINCE_H
