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
  Vertex(float a, float b) : x(a), y(b) {}
  Vertex(const Vertex& copy) : x(copy.x), y(copy.y) {}
  void add(float);
  void multiply(float);
  float x;
  float y;
};

struct NS_DECLSPEC Triangle {
  Triangle() {
    v[0] = Vertex();
    v[1] = Vertex();
    v[2] = Vertex();
    t[0] = Vertex();
    t[1] = Vertex();
    t[2] = Vertex();
  }

  Vertex v[3];
  Vertex t[3];
};

struct NS_DECLSPEC Triangle_2 {
  Triangle_2() {
    v[0] = Vertex();
    v[1] = Vertex();
    v[2] = Vertex();
    t_back[0] = Vertex();
    t_back[1] = Vertex();
    t_back[2] = Vertex();
    t_model[0] = Vertex();
    t_model[1] = Vertex();
    t_model[2] = Vertex();
  }
  Vertex v[3];
  Vertex t_back[3];
  Vertex t_model[3];
};

std::ostream& NS_DECLSPEC operator<<(std::ostream& stream, const Event e);
std::istream& NS_DECLSPEC operator>>(std::istream&, Vertex&);
std::istream& NS_DECLSPEC operator>>(std::istream&, Triangle&);
std::istream& NS_DECLSPEC operator>>(std::istream&, Triangle_2&);

class IEngine;
IEngine* NS_DECLSPEC create_engine();
void NS_DECLSPEC delete_engine(IEngine*);

class NS_DECLSPEC IEngine {
 public:
  virtual ~IEngine();
  virtual int finish() = 0;
  virtual void render_quad(const Triangle_2&, const Triangle_2&) = 0;
  virtual void render_triangle(const Triangle&) = 0;
  virtual void render_triangle(const Triangle_2&) = 0;
  virtual void render_triangle_minimap(const Triangle_2&) = 0;
  virtual void swap_buffers() = 0;
  virtual bool read_event(Event& event) = 0;
  virtual std::string init(const std::string& config) = 0;
  virtual float get_time() = 0;
  //  virtual bool load_texture(std::string path) = 0;
};

}  // namespace ns
