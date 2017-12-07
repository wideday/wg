#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

#include "engine.h"

std::string read_config(const std::string file_name) {
  std::ifstream mol_file(file_name);
  std::string mol_string((std::istreambuf_iterator<char>(mol_file)),
                         std::istreambuf_iterator<char>());
  return std::string();
}

int main(int /*argc*/, char* /*argv*/ []) {
  std::unique_ptr<ns::IEngine, void (*)(ns::IEngine*)> engine(
      ns::create_engine(), ns::delete_engine);
  std::string init_result = engine->init(read_config("keys.txt"));
  if (!init_result.empty()) return EXIT_FAILURE;

  bool continue_loop = true;
  while (continue_loop) {
    ns::Event event;

    while (engine->read_event(event)) {
      // std::cout << Event << std::endl;
      switch (event) {
        case ns::Event::turn_off:
          continue_loop = false;
          break;

        default:
          break;
      }
    }

    std::ifstream file("vertexes.txt");
    assert(!!file);
    ns::Triangle_2 tr1;
    ns::Triangle_2 tr2;
    file >> tr1 >> tr2;

    float time = engine->get_time();
    float s = sin(time) / 5;
    float c = cos(time) / 5;

    for (auto& v : tr1.t_back) {
      v.x += c;
      v.y += s;
    }
    for (auto& v : tr2.t_back) {
      v.x += c;
      v.y += s;
    }

    engine->render_triangle(tr2);
    engine->render_triangle(tr1);

    engine->swap_buffers();
  }

  engine->finish();

  return EXIT_SUCCESS;
}
