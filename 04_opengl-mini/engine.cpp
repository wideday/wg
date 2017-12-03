#include "engine.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#if __MINGW32__
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles2.h>
#endif

#define ENGINE_GL_CHECK()                                             \
                                                                      \
  {                                                                   \
    const int err = glGetError();                                     \
    if (err != GL_NO_ERROR) {                                         \
      switch (err) {                                                  \
        case GL_INVALID_ENUM:                                         \
          std::cerr << GL_INVALID_ENUM << std::endl;                  \
          break;                                                      \
        case GL_INVALID_VALUE:                                        \
          std::cerr << GL_INVALID_VALUE << std::endl;                 \
          break;                                                      \
        case GL_INVALID_OPERATION:                                    \
          std::cerr << GL_INVALID_OPERATION << std::endl;             \
          break;                                                      \
        case GL_INVALID_FRAMEBUFFER_OPERATION:                        \
          std::cerr << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl; \
          break;                                                      \
        case GL_OUT_OF_MEMORY:                                        \
          std::cerr << GL_OUT_OF_MEMORY << std::endl;                 \
          break;                                                      \
      }                                                               \
      assert(false);                                                  \
    }                                                                 \
  }

namespace ns {

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
const char* WINDOW_TITLE = "Title";
std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

std::istream& operator>>(std::istream& is, Vertex& v) {
  is >> v.x;
  is >> v.y;
  return is;
}

std::istream& operator>>(std::istream& is, Triangle& t) {
  is >> t.v[0];
  is >> t.v[1];
  is >> t.v[2];
  return is;
}

// std::ostream& operator<<(std::ostream& stream, const Event e) {
// stream << keys[e].name;
//}

struct bind {
  bind(SDL_Keycode k, std::string s, Event pressed, Event released)
      : key(k), name(s), event_pressed(pressed), event_released(released) {}

  SDL_Keycode key;
  std::string name;
  Event event_pressed;
  Event event_released;
};

void set_keys(const std::string& keys_string) {
  if (!keys_string.empty()) return;
  return;
}

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
  }
}

bool check_SDL_version(void) {
  SDL_version compiled = {0, 0, 0};
  SDL_version linked = {0, 0, 0};

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  bool result = true;
  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    std::cerr << "warning: SDL2 compiled and linked version mismatch: "
              << compiled << " " << linked << std::endl;
    result = false;
  }
  return result;
}

bool check_GL_version(void) {
  int gl_major_ver = 0;
  int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
  assert(result == 0);

  int gl_minor_ver = 0;
  result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
  assert(result == 0);

  std::clog << "gl: " << gl_major_ver << '.' << gl_minor_ver << std::endl;
  //  std::exit(0);
  return true;
}

class Engine_impl final : public IEngine {
 public:
  std::string init(const std::string& config) final {
    check_SDL_version();

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0) {
      const char* err_message = SDL_GetError();
      std::cerr << "error: failed call SDl_Init: " << err_message << std::endl;
      return "";
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                              WINDOW_HEIGHT, ::SDL_WINDOW_OPENGL);
    if (window == nullptr) {
      const char* err_message = SDL_GetError();
      std::cerr << "error: failed call SDl_CreateWindow: " << err_message
                << std::endl;
      SDL_Quit();
      return "";
    }

    set_keys(config);

    // TODO: set attributes for version
    gl_context = SDL_GL_CreateContext(window);
    assert(gl_context != nullptr);
    check_GL_version();

    if (glewInit()) {
      std::cerr << "Unable to initialize GLEW ... exiting\n";
      SDL_GL_DeleteContext(gl_context);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return "";
    }

    /* Vertex shader */
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //"attribute vec2 coord2d;\n"
    static const GLchar* vertex_shader_source =
        "#version 120\n"
        "attribute vec2 coord2d;\n"
        "void main() {\n"
        "    gl_Position = vec4(coord2d, 0.0, 1.0);\n"
        "}\n";
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    GLint compile_success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_success);
    if (!compile_success) {
      GLint log_len;
      glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_len);
      std::vector<GLchar> log(log_len);
      glGetShaderInfoLog(vertex_shader, log_len, NULL, log.data());
      glDeleteShader(vertex_shader);
      std::cerr << "Vertex shader error: " << log.data() << std::endl;
      return "";
    }

    /* Fragment shader */
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    static const GLchar* fragment_shader_source =
        "#version 120\n"
        "void main() {\n"
        "    gl_FragColor = vec4(0.3, 0.1, 0.5, 1.0);\n"
        "}\n";
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_success);
    if (!compile_success) {
      GLint log_len;
      glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_len);
      std::vector<GLchar> log(log_len);
      glGetShaderInfoLog(fragment_shader, log_len, NULL, log.data());
      glDeleteShader(fragment_shader);
      std::cerr << "Fragment shader error: " << log.data() << std::endl;
      return "";
    }

    /* Link shaders */
    GLuint program = glCreateProgram();
    if (program == 0) {
      std::cerr << "Create program error." << std::endl;
      return "";
    }
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glBindAttribLocation(program, 0, "coord2d");
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &compile_success);
    if (!compile_success) {
      GLint log_len;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
      std::vector<GLchar> log(log_len);
      glGetProgramInfoLog(program, log_len, NULL, log.data());
      glDeleteProgram(program);
      std::cerr << "Link Program error: " << log.data() << std::endl;
      return "";
    }

    /* Cleanup. */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(program);
    // The End

    //    static const GLuint WIDTH = 512;
    //    static const GLuint HEIGHT = 512;
    //
    //    static GLfloat vertices[] = {
    //        0.0, 0.8, -0.8, -0.8, 0.8, -0.8,
    //    };
    //
    //    /* Shader setup. */
    //    // program =
    //    //   common_get_shader_program(vertex_shader_source,
    //    //   fragment_shader_source);
    //
    //    /* Buffer setup. */
    //    GLuint vbo;
    //    glGenBuffers(1, &vbo);
    //    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
    //    GL_STATIC_DRAW);
    //
    //    GLint attribute_coord2d = glGetAttribLocation(program, "coord2d");
    //    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //    glEnableVertexAttribArray(0);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //
    //    /* Global draw state */
    //
    //    glViewport(0, 0, WIDTH, HEIGHT);
    //    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return "";
  }

  void render_triangle(const Triangle& t) final {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    ENGINE_GL_CHECK();
    glClear(GL_COLOR_BUFFER_BIT);
    ENGINE_GL_CHECK();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &t.v[0]);
    ENGINE_GL_CHECK();
    glEnableVertexAttribArray(0);
    ENGINE_GL_CHECK();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_GL_CHECK();
    glDisableVertexAttribArray(0);
    ENGINE_GL_CHECK();
  }
  void swap_buffers() final { SDL_GL_SwapWindow(window); }
  bool read_event(Event& e) final {
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_KEYDOWN:
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
  int finish() final {
    // glDeleteBuffers(1, &vbo);
    // glDeleteProgram(program);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
  }

 private:
  SDL_Window* window = nullptr;
  SDL_GLContext gl_context = nullptr;
};

IEngine* create_engine() {
  IEngine* engine = new Engine_impl();
  return engine;
}

void delete_engine(IEngine* engine) { delete engine; }
IEngine::~IEngine() {}

}  // end namespace ns
