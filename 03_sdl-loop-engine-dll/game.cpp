#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "engine.h"

std::string read_config(const std::string file_name) {
  std::ifstream mol_file(file_name);
  std::string mol_string((std::istreambuf_iterator<char>(mol_file)),
                         std::istreambuf_iterator<char>());
  return mol_string;
}

int main(int /*argc*/, char* /*argv*/ []) {
  std::string init_result = engine::init(read_config("keys.txt"));
  if (!init_result.empty()) return EXIT_FAILURE;

  bool continue_loop = true;
  while (continue_loop) {
    engine::Event event;

    while (engine::read_event(event)) {
      // std::cout << Event << std::endl;
      switch (event) {
        case engine::Event::turn_off:
          continue_loop = false;
          break;

        default:
          break;
      }
    }
  }

  engine::finish();

  return EXIT_SUCCESS;
}
