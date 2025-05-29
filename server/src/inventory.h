#include <cstdint>
#include <map>
#include <string>
#include <vector>

class Inventory {
public:
  void setQuantity(const std::string& name, uint32_t quantity);

  void remove(const std::string& name);
  std::vector<uint8_t> encode();

private:
  std::map<std::string, uint32_t> entries;
  unsigned int byteSize;
};
