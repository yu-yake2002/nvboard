#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>
#include <nvboard.h>
#include <sys/time.h>

#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <string>
#include <fstream>

#define FPS 60

static uint64_t boot_time = 0;

static uint64_t get_time_internal() {
  struct timeval now;
  gettimeofday(&now, NULL);
  uint64_t us = now.tv_sec * 1000000 + now.tv_usec;
  return us;
}

static uint64_t get_time() {
  uint64_t now = get_time_internal();
  return now - boot_time;
}

static SDL_Renderer *main_renderer = nullptr;
uint64_t input_map[NR_INPUT_PINS] = {0};
uint64_t output_map[NR_OUTPUT_PINS] = {0};

int read_event();

void NVBoard::nvboard_update_input(PinMap *p) {
  void *ptr = p->signal;
  if (p->len == 1) {
    uint8_t val = input_map[p->pin];
    *(uint8_t *)ptr = val;
    return;
  }

  int len = p->len;
  uint64_t val = 0;
  for (int i = 0; i < len; i ++) {
    val <<= 1;
    val |= input_map[p->pins[i]];
  }
  if (len <= 8) { *(uint8_t *)ptr = val; }
  else if (len <= 16) { *(uint16_t *)ptr = val; }
  else if (len <= 32) { *(uint32_t *)ptr = val; }
  else if (len <= 64) { *(uint64_t *)ptr = val; }
}

void NVBoard::nvboard_update_output(PinMap *p) {
  void *ptr = p->signal;
  if (p->len == 1) {
    uint8_t val = *(uint8_t *)ptr;
    output_map[p->pin] = val & 1;
    return;
  }

  int len = p->len;
  uint64_t val = 0;
  if (len <= 8) { val = *(uint8_t *)ptr; }
  else if (len <= 16) { val = *(uint16_t *)ptr; }
  else if (len <= 32) { val = *(uint32_t *)ptr; }
  else if (len <= 64) { val = *(uint64_t *)ptr; }
  for (int i = 0; i < len; i ++) {
    output_map[p->pins[i]] = val & 1;
    val >>= 1;
  }
}

int NVBoard::nvboard_update() {
  for (auto p : this->rt_pin_map_v) {
    if (p->is_output) nvboard_update_output(p);
    else nvboard_update_input(p);
  }

  update_rt_components(main_renderer);
  
  int ret = 1;
  static uint64_t last = 0;
  uint64_t now = get_time();
  if (now - last > 1000000 / FPS) {
    last = now;

    for (auto p : this->pin_map_v) {
      if (p->is_output) nvboard_update_output(p);
      else nvboard_update_input(p);
    }

    int ev = read_event();
    if (ev == -1) { ret = 0; }

    update_components(main_renderer);
    this->renderer->RendererUpdate();
  }
  return ret;
}

NVBoard::NVBoard(int vga_clk_cycle, std::string board_name) {
  printf("NVBoard v0.2\n");
  // init SDL and SDL_image
  SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  IMG_Init(IMG_INIT_PNG);

  std::string nvboard_home = getenv("NVBOARD_HOME");
  std::ifstream ifs(nvboard_home + "/board/" + board_name + "/config.json");
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj);
  
  this->renderer = new NVBoardRenderer(obj);
  main_renderer = this->renderer->GetRenderer();
  
  init_gui(main_renderer);

  update_components(main_renderer);
  update_rt_components(main_renderer);

  boot_time = get_time_internal();
  extern void vga_set_clk_cycle(int cycle);
  vga_set_clk_cycle(vga_clk_cycle);
}

NVBoard::~NVBoard(){
  delete_components();
  delete this->renderer;
  for (auto p : this->pin_map_v) {
    delete p;
  }
  for (auto p : this->rt_pin_map_v) {
    delete p;
  }
  IMG_Quit();
  SDL_Quit();
}

void NVBoard::nvboard_bind_pin(void *signal, bool is_rt, bool is_output, int len, ...) {
  PinMap *p = new PinMap;
  p->is_output = is_output;
  p->len = len;
  assert(len < 64);

  va_list ap;
  va_start(ap, len);
  if (len == 1) { p->pin = (uint16_t)va_arg(ap, int); }
  else {
    p->pins = new uint16_t[p->len];
    for (int i = 0; i < len; i ++) {
      uint16_t pin = va_arg(ap, int);
      if (is_output) p->pins[len - 1 - i] = pin;
      else p->pins[i] = pin;
    }
  }
  va_end(ap);

  p->signal = signal;
  if (is_rt) {
    this->rt_pin_map_v.push_back(p);
  } else {
    this->pin_map_v.push_back(p);
  }
}
