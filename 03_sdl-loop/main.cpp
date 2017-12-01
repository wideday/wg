#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

#include <SDL2/SDL.h>

std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

struct bind {
  bind(SDL_Keycode k, std::string s) : key(k), name(s) {}

  SDL_Keycode key;
  std::string name;
};

void log_input(const SDL_Event& e, std::ostream& out = std::cout) {
  std::array<bind, 8> keys{
      bind{SDLK_w, "up"},        bind{SDLK_a, "left"},
      bind{SDLK_s, "down"},      bind{SDLK_d, "right"},
      bind{SDLK_TAB, "tab"},     bind{SDLK_SPACE, "space"},
      bind{SDLK_ESCAPE, "exit"}, bind{SDLK_RETURN, "enter"}};

  const auto it = std::find_if(begin(keys), end(keys), [&](const bind& b) {
    return b.key == e.key.keysym.sym;
  });

  if (it != end(keys)) {
    out << "Key \"" << it->name << "\" ";
    if (e.type == SDL_KEYDOWN) {
      out << "down.\n";
    } else {
      out << "up.\n";
    }
    out << std::endl;
  }
}

int main(int /*argc*/, char* /*argv*/ []) {
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
    return EXIT_FAILURE;
  }

  SDL_Window* const window =
      SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       640, 480, ::SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char* err_message = SDL_GetError();
    std::cerr << "error: failed call SDl_CreateWindow: " << err_message
              << std::endl;
    SDL_Quit();
    return EXIT_FAILURE;
  }

  bool continue_loop = true;
  while (continue_loop) {
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_KEYDOWN:
          if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
            continue_loop = false;
          }
          log_input(sdl_event);
          break;
        case SDL_KEYUP:
          log_input(sdl_event);
          break;
        case SDL_QUIT:
          continue_loop = false;
          break;
        default:
          break;
      }
    }
  }

  SDL_DestroyWindow(window);

  SDL_Quit();

  return EXIT_SUCCESS;
}
