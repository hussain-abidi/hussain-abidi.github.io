#include "server.h"
#include <csignal>
#include <cstring>

const int PORT = 3000;
Inventory inv;

int main() {
  signal(SIGINT, server::cleanup);

  server::run(PORT, &inv);
}

void Inventory::setQuantity(const std::string& name, uint32_t quantity) {
  if (quantity == 0) {
    entries.erase(name);
    return;
  }

  entries[name] = quantity;
  byteSize += sizeof(uint32_t) + name.size() + sizeof(uint32_t);
}

void Inventory::remove(const std::string& name) {
  auto it = entries.find(name);
  if (it != entries.end()) {
    byteSize -= sizeof(uint32_t) + name.size() + sizeof(uint32_t);
    entries.erase(it);
  }
}
std::vector<uint8_t> Inventory::encode() {
  std::vector<uint8_t> buffer(sizeof(uint32_t) + byteSize);
  uint8_t* ptr = buffer.data();

  uint32_t entriesLength = entries.size();

  memcpy(ptr, &entriesLength, sizeof(uint32_t));
  ptr += sizeof(uint32_t);

  for (auto& entry : entries) {
    const std::string& name = entry.first;
    unsigned long quantity = entry.second;

    uint32_t nameLength = name.size();
    memcpy(ptr, &nameLength, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(ptr, name.data(), name.size());
    ptr += name.size();

    memcpy(ptr, &quantity, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
  }

  return buffer;
}
