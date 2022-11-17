#ifndef V3_PROVINCE_H
#define V3_PROVINCE_H
#include <memory>
#include <string>

namespace V3
{
struct Chunk;
class Province
{
  public:
	Province() = default;

	[[nodiscard]] auto getName() const { return name; }
	[[nodiscard]] auto getTerrain() const { return terrain; }
	[[nodiscard]] auto getChunk() const { return chunk; }
	[[nodiscard]] auto isSea() const { return sea; }
	[[nodiscard]] auto isLake() const { return lake; }
	[[nodiscard]] auto isImpassable() const { return impassable; }

	void setSea() { sea = true; }
	void setLake()
	{
		lake = true;
		impassable = true;
	}
	void setImpassable() { impassable = true; }
	void setName(const std::string& theName) { name = theName; }
	void setTerrain(const std::string& theTerrain) { terrain = theTerrain; }
	void setChunk(const std::shared_ptr<Chunk>& theChunk) { chunk = theChunk; }

  private:
	void registerKeys();

	std::string name;
	std::string terrain;
	bool sea = false;
	bool lake = false;
	bool impassable = false;
	std::shared_ptr<Chunk> chunk;
};
} // namespace V3

#endif // V3_PROVINCE_H
