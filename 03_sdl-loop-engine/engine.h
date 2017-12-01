#pragma once
#include <string>

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

std::ostream& operator<<(std::ostream& stream, const Event& event);

std::string init(const std::string& config);

bool read_event(Event& event);

int finish();
}  // namespace engine
