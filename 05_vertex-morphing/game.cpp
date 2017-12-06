#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "engine.h"

ns::Vertex blend_vertex(const ns::Vertex& v_l, const ns::Vertex& v_r,
                        const float a) {
  ns::Vertex r;
  r.x = (1.0f - a) * v_l.x + a * v_r.x;
  r.y = (1.0f - a) * v_l.y + a * v_r.y;
  return r;
}

ns::Triangle blend(const ns::Triangle& tr_q, const ns::Triangle& tr_t,
                   const float a) {
  ns::Triangle r;
  r.v[0] = blend_vertex(tr_q.v[0], tr_t.v[0], a);
  r.v[1] = blend_vertex(tr_q.v[1], tr_t.v[1], a);
  r.v[2] = blend_vertex(tr_q.v[2], tr_t.v[2], a);
  return r;
}

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

  std::ifstream file("vertexes.txt");
  assert(!!file);

  ns::Triangle tr1q;
  ns::Triangle tr2q;
  ns::Triangle tr1t;
  ns::Triangle tr2t;
  file >> tr1q >> tr2q >> tr1t >> tr2t;

  float alpha = 0.0f;
  float step = 0.05f;
  int sleep_time = 100;

  while (continue_loop) {
    ns::Event event;

    while (engine->read_event(event)) {
      // std::cout << Event << std::endl;
      switch (event) {
        case ns::Event::turn_off:
          continue_loop = false;
          break;
        case ns::Event::select_pressed:
          continue_loop = false;
          break;
        default:
          break;
      }
    }

    if (alpha < 0.0f || alpha > 1.0f) {
      step = -step;
    }
    alpha -= step;
    ns::Triangle tr1 = blend(tr1q, tr1t, alpha);
    ns::Triangle tr2 = blend(tr2q, tr2t, alpha);

    engine->render_triangle(tr1);
    engine->render_triangle(tr2);

    engine->swap_buffers();

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
  }

  engine->finish();

  return EXIT_SUCCESS;
}
