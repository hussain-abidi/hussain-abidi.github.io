// currenty unused

#include <stack>
class IDPool {
public:
  IDPool();
  ~IDPool();

  unsigned long get();
  void replace(unsigned long elem);

private:
  std::stack<unsigned long> pool;
  int sz;
};
