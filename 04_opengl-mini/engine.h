#pragma once
#include <string>

#ifndef NS_DECLSPEC
#define NS_DECLSPEC
#endif

namespace ns {

enum class Event {
  left_pressed,
  left_released,
  right_pressed,
  right_released,
  up_pressed,
  up_released,
  down_pressed,
  down_released,
  select_pressed,
  select_released,
  start_pressed,
  start_released,
  button1_pressed,
  button1_released,
  button2_pressed,
  button2_released,
  turn_off
};

struct Vertex {
  Vertex() : x(0.f), y(0.f) {}
  float x;
  float y;
};

struct NS_DECLSPEC Triangle {
  Triangle() {
    v[0] = Vertex();
    v[1] = Vertex();
    v[2] = Vertex();
  }
  Vertex v[3];
};

std::ostream& NS_DECLSPEC operator<<(std::ostream& stream, const Event e);
std::istream& NS_DECLSPEC operator>>(std::istream&, Vertex&);
std::istream& NS_DECLSPEC operator>>(std::istream&, Triangle&);

class IEngine;
IEngine* NS_DECLSPEC create_engine();
void NS_DECLSPEC delete_engine(IEngine*);

class NS_DECLSPEC IEngine {
 public:
  virtual ~IEngine();
  virtual int finish() = 0;
  virtual void render_triangle(const Triangle&) = 0;
  virtual void swap_buffers() = 0;
  virtual bool read_event(Event& event) = 0;
  virtual std::string init(const std::string& config) = 0;
};

}  // namespace ns
