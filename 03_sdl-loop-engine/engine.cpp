#include "engine.h"

#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <SDL2/SDL.h>

namespace engine {

struct bind {
  bind(SDL_Keycode k, std::string s, Event pressed, Event released)
      : key(k), name(s), event_pressed(pressed), event_released(released) {}
  std::string get_name() {
    std::string result = name + "" + "";
    return result;
  }

  SDL_Keycode key;
  std::string name;
  Event event_pressed;
  Event event_released;
};

std::array<bind, 8> keys{
    bind{SDLK_w, "up", Event::up_pressed, Event::up_released},
    bind{SDLK_a, "left", Event::left_pressed, Event::left_released},
    bind{SDLK_s, "down", Event::down_pressed, Event::down_released},
    bind{SDLK_d, "right", Event::right_pressed, Event::right_released},
    bind{SDLK_LCTRL, "button1", Event::button1_pressed,
         Event::button1_released},
    bind{SDLK_SPACE, "button2", Event::button2_pressed,
         Event::button2_released},
    bind{SDLK_ESCAPE, "select", Event::select_pressed, Event::select_released},
    bind{SDLK_RETURN, "start", Event::start_pressed, Event::start_released}};

// std::ostream& operator<<(std::ostream& stream, const Event& event) {
//  //  stream << keys[event].name;
//  return stream;
//}

std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

void set_keys(const std::string& keys_string) {
  if (!keys_string.empty()) return;
  return;
}

void check_input(const SDL_Event& e_sdl, Event& e_out,
                 std::ostream& out = std::cout) {
  const auto it = std::find_if(begin(keys), end(keys), [&](const bind& b) {
    return b.key == e_sdl.key.keysym.sym;
  });

  if (it != end(keys)) {
    out << "Key \"" << it->name << "\" ";
    if (e_sdl.type == SDL_KEYDOWN) {
      out << "down.\n";
      e_out = it->event_pressed;
    } else {
      out << "up.\n";
      e_out = it->event_released;
    }
    out.flush();
  }
}

std::string init(const std::string& config) {
  SDL_version compiled = {0, 0, 0};
  SDL_version linked = {0, 0, 0};

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    std::cerr << "warning: SDL2 compiled and linked version mismatch: "
              << compiled << " " << linked << std::endl;
  }

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    const char* err_message = SDL_GetError();
    std::cerr << "error: failed call SDl_Init: " << err_message << std::endl;
    return "";
  }

  SDL_Window* const window =
      SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       640, 480, ::SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char* err_message = SDL_GetError();
    std::cerr << "error: failed call SDl_CreateWindow: " << err_message
              << std::endl;
    SDL_Quit();
    return "";
  }
  set_keys(config);
  return "";
}

int finish() {
  //  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

bool read_event(Event& e) {
  SDL_Event sdl_event;
  if (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
      case SDL_KEYDOWN:
        check_input(sdl_event, e);
        break;
      case SDL_KEYUP:
        check_input(sdl_event, e);
        break;
      case SDL_QUIT:
        e = Event::turn_off;
        break;
      default:
        break;
    }

    return true;
  }
  return false;
}

}  // namespace engine
