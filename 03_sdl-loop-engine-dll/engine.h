#pragma once
#include <string>

#ifndef ENGINE_DECLSPEC
#define ENGINE_DECLSPEC
#endif

//#ifdef _WIN32
//#ifdef BUILD_ENGINE
//#define DECLSPEC __declspec(dllexport)
//#else
//#define DECLSPEC __declspec(dllimport)
//#endif
//#else
//#define DECLSPEC
//#endif

namespace engine {

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

std::ostream& ENGINE_DECLSPEC operator<<(std::ostream& stream, const Event e);

std::string ENGINE_DECLSPEC init(const std::string& config);

bool ENGINE_DECLSPEC read_event(Event& event);

int ENGINE_DECLSPEC finish();
}  // namespace engine
