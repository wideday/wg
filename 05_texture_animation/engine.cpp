#include "engine.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "picopng.cpp"

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
//#if __MINGW32__
//#include <SDL2/SDL_opengl.h>
//#else
//#include <SDL2/SDL_opengles2.h>
//#endif

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

Vertex& Vertex::add(float a) {
  this->x += a;
  this->y += a;
  return *this;
}
Vertex& Vertex::multiply(float a) {
  this->x *= a;
  this->y *= a;
  return *this;
}

void Triangle_2::init(float koef) {
  for (size_t i = 0; i < 3; ++i) {
    this->t_back[i] =
        Vertex(this->t_model[i]).multiply(koef).add((1 - koef) / 2);
  }
}

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
  is >> t.t[0];
  is >> t.v[1];
  is >> t.t[1];
  is >> t.v[2];
  is >> t.t[2];
  return is;
}

std::istream& operator>>(std::istream& is, Triangle_2& t) {
  is >> t.v[0];
  //  is >> t.t_back[0];
  is >> t.t_model[0];
  is >> t.v[1];
  //  is >> t.t_back[1];
  is >> t.t_model[1];
  is >> t.v[2];
  //  is >> t.t_back[2];
  is >> t.t_model[2];
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
    ENGINE_GL_CHECK();
    static const GLchar* vertex_shader_source =
        "#version 120\n"
        "attribute vec2 a_coord2d;\n"
        "attribute vec2 a_texture2d;\n"
        "varying vec2 v_TexCoord;\n"
        "void main() {\n"
        "	gl_Position = vec4(a_coord2d, 0.0, 1.0);\n"
        "	v_TexCoord = vec2(a_texture2d.x, 1.0f - a_texture2d.y);\n"
        "}\n";
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    ENGINE_GL_CHECK();
    glCompileShader(vertex_shader);
    ENGINE_GL_CHECK();

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
    ENGINE_GL_CHECK();
    static const GLchar* fragment_shader_source =
        "#version 120\n"
        "varying vec2 v_TexCoord;\n"
        "uniform sampler2D u_ourTexture;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(u_ourTexture, v_TexCoord);\n"
        "}\n";
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    ENGINE_GL_CHECK();
    glCompileShader(fragment_shader);
    ENGINE_GL_CHECK();

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
    ENGINE_GL_CHECK();
    if (program == 0) {
      std::cerr << "Create program error." << std::endl;
      return "";
    }
    glAttachShader(program, vertex_shader);
    ENGINE_GL_CHECK();
    glAttachShader(program, fragment_shader);
    ENGINE_GL_CHECK();
    glBindAttribLocation(program, 0, "a_coord2d");
    ENGINE_GL_CHECK();
    glBindAttribLocation(program, 1, "a_texture2d");
    ENGINE_GL_CHECK();

    glLinkProgram(program);
    ENGINE_GL_CHECK();

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
    ENGINE_GL_CHECK();
    glDeleteShader(fragment_shader);
    ENGINE_GL_CHECK();

    glUseProgram(program);
    ENGINE_GL_CHECK();

    texture_back = load_texture("sand_brown.png", 0);
    texture_model = load_texture("tank.png", 0);
    texture_up = load_texture("clouds.png", 0);
    GLint textureLocation = glGetUniformLocation(program, "u_ourTexture");
    ENGINE_GL_CHECK();
    glActiveTexture(GL_TEXTURE0);
    ENGINE_GL_CHECK();

    glUniform1i(textureLocation, 0);
    ENGINE_GL_CHECK();

    glEnable(GL_BLEND);
    ENGINE_GL_CHECK();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ENGINE_GL_CHECK();
    // The End

    return "";
  }

  GLuint load_texture(std::string path, size_t texture_number) {
    std::vector<unsigned char> png_file_in_memory;
    std::ifstream ifs(path, std::ios_base::binary);
    if (!ifs) return false;
    ifs.seekg(0, std::ios_base::end);
    size_t pos_in_file = ifs.tellg();
    png_file_in_memory.resize(pos_in_file);
    ifs.seekg(0, std::ios_base::beg);

    ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()), pos_in_file);
    if (!ifs) return false;

    std::vector<unsigned char> image;
    unsigned long w = 0;
    unsigned long h = 0;
    int error = decodePNG(image, w, h, &png_file_in_memory[0],
                          png_file_in_memory.size());

    if (error != 0) {
      std::cerr << "error: " << error << std::endl;
    } else {
      std::cout << "Loaded image successfull" << std::endl;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    ENGINE_GL_CHECK();
    glActiveTexture(GL_TEXTURE0 + texture_number);
    glBindTexture(GL_TEXTURE_2D, texture);
    ENGINE_GL_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ENGINE_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ENGINE_GL_CHECK();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 &image[0]);

    ENGINE_GL_CHECK();

    //    glGenerateMipmap(GL_TEXTURE_2D);
    //    ENGINE_GL_CHECK();
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //    ENGINE_GL_CHECK();

    return texture;
  }

  void render_triangle(Vertex const* vertex, Vertex const* textur,
                       GLuint texture) {
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertex[0]);
    ENGINE_GL_CHECK();
    glEnableVertexAttribArray(0);
    ENGINE_GL_CHECK();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &textur[0]);
    ENGINE_GL_CHECK();
    glEnableVertexAttribArray(1);
    ENGINE_GL_CHECK();

    glBindTexture(GL_TEXTURE_2D, texture);
    ENGINE_GL_CHECK();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_GL_CHECK();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    ENGINE_GL_CHECK();
  }

  void render_triangle(const Triangle& t) final {
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &t.v[0]);
    ENGINE_GL_CHECK();
    glEnableVertexAttribArray(0);
    ENGINE_GL_CHECK();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &t.t[0]);
    ENGINE_GL_CHECK();
    glEnableVertexAttribArray(1);
    ENGINE_GL_CHECK();

    glBindTexture(GL_TEXTURE_2D, texture_back);
    ENGINE_GL_CHECK();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_GL_CHECK();
    glBindTexture(GL_TEXTURE_2D, texture_model);
    ENGINE_GL_CHECK();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_GL_CHECK();
    glBindTexture(GL_TEXTURE_2D, texture_up);
    ENGINE_GL_CHECK();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_GL_CHECK();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    ENGINE_GL_CHECK();
  }

  void render_triangle(const Triangle_2& t) final {
    render_triangle(t.v, t.t_back, texture_back);

    render_triangle(t.v, t.t_model, texture_model);

    render_triangle(t.v, t.t_back, texture_up);
  }

  void render_triangle_minimap(const Triangle_2& t) final {
    Vertex new_v[3] = {Vertex(t.v[0]), Vertex(t.v[1]), Vertex(t.v[2])};
    for (Vertex& element : new_v) {
      element.add(0.5f);
      element.multiply(0.2f);
      element.add(-0.5f);
    }
    Vertex new_v_minimodel[3] = {Vertex(t.t_back[0]), Vertex(t.t_back[1]),
                                 Vertex(t.t_back[2])};
    for (Vertex& element : new_v_minimodel) {
      element.multiply(0.2f);
      element.add(-0.5f);
    }
    render_triangle(new_v, t.t_model, texture_back);
    render_triangle(new_v_minimodel, t.t_model, texture_model);
    render_triangle(new_v, t.t_model, texture_up);
  }

  void render_quad(const Triangle_2& tr1, const Triangle_2& tr2,
                   const float koef_minimap) {
    render_triangle(tr1.v, tr1.t_back, texture_back);
    render_triangle(tr2.v, tr2.t_back, texture_back);

    render_triangle(tr1.v, tr1.t_model, texture_model);
    render_triangle(tr2.v, tr2.t_model, texture_model);

    render_triangle(tr1.v, tr1.t_back, texture_up);
    render_triangle(tr2.v, tr2.t_back, texture_up);

    Vertex new_v_1[3] = {Vertex(tr1.v[0]), Vertex(tr1.v[1]), Vertex(tr1.v[2])};
    for (Vertex& element : new_v_1) {
      element.add(0.5f);
      element.multiply(koef_minimap);
      element.add(-0.5f);
    }
    render_triangle(new_v_1, tr1.t_model, texture_back);
    Vertex new_v_2[3] = {Vertex(tr2.v[0]), Vertex(tr2.v[1]), Vertex(tr2.v[2])};
    for (Vertex& element : new_v_2) {
      element.add(0.5f);
      element.multiply(koef_minimap);
      element.add(-0.5f);
    }
    render_triangle(new_v_2, tr2.t_model, texture_back);

    Vertex new_v_minimodel_1[3] = {Vertex(tr1.t_back[0]), Vertex(tr1.t_back[1]),
                                   Vertex(tr1.t_back[2])};
    for (Vertex& element : new_v_minimodel_1) {
      element.multiply(koef_minimap);
      element.add(-0.5f);
    }
    render_triangle(new_v_minimodel_1, tr1.t_model, texture_model);
    Vertex new_v_minimodel_2[3] = {Vertex(tr2.t_back[0]), Vertex(tr2.t_back[1]),
                                   Vertex(tr2.t_back[2])};
    for (Vertex& element : new_v_minimodel_2) {
      element.multiply(koef_minimap);
      element.add(-0.5f);
    }
    render_triangle(new_v_minimodel_2, tr2.t_model, texture_model);

    render_triangle(new_v_1, tr1.t_model, texture_up);
    render_triangle(new_v_2, tr2.t_model, texture_up);
  }

  void swap_buffers() final {
    SDL_GL_SwapWindow(window);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    ENGINE_GL_CHECK();
    glClear(GL_COLOR_BUFFER_BIT);
    ENGINE_GL_CHECK();
  }
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

  float get_time() final { return SDL_GetTicks() * 0.001f; }

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
  GLuint texture_back = 0;
  GLuint texture_model = 0;
  GLuint texture_up = 0;
};

IEngine* create_engine() {
  IEngine* engine = new Engine_impl();
  return engine;
}

void delete_engine(IEngine* engine) { delete engine; }
IEngine::~IEngine() {}

}  // end namespace ns
