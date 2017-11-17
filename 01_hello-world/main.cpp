#include <cstdlib>
#include <iostream>

int main(int /*argc*/, char* /*argv*/ []) {
  std::cout << "Hello, world!" << std::endl;
  return std::cout.good() ? EXIT_SUCCESS : EXIT_FAILURE;
}
