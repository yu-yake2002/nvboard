#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>
#include <nvboard.h>
#include <sys/time.h>

#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <string>

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

uint64_t input_map[NR_INPUT_PINS] = {0};
uint64_t output_map[NR_OUTPUT_PINS] = {0};

void NVBoardController::UpdateInputPin(PinMap *p) {
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

void NVBoardController::UpdateOutputPin(PinMap *p) {
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

int NVBoardController::Update() {
  for (auto p : rt_pin_map_v_) {
    if (p->is_output) UpdateOutputPin(p);
    else UpdateInputPin(p);
  }

  viewer_->UpdateRTComponents();
  
  int ret = 1;
  static uint64_t last = 0;
  uint64_t now = get_time();
  if (now - last > 1000000 / FPS) {
    last = now;

    for (auto p : pin_map_v_) {
      if (p->is_output) UpdateOutputPin(p);
      else UpdateInputPin(p);
    }

    if (viewer_->read_event() == -1) { ret = 0; }

    viewer_->UpdateNotRTComponents();
    viewer_->RendererUpdate();
  }
  return ret;
}

NVBoardController::NVBoardController(NVBoardView *viewer) {
  printf("NVBoard v0.2\n");
  viewer_ = viewer;
  boot_time = get_time_internal();
}

NVBoardController::~NVBoardController(){
  for (auto p : pin_map_v_) {
    delete p;
  }
  for (auto p : rt_pin_map_v_) {
    delete p;
  }
  IMG_Quit();
  SDL_Quit();
}

void NVBoardController::NVboardBindPin(void *signal, bool is_rt, bool is_output, int len, ...) {
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
    rt_pin_map_v_.push_back(p);
  } else {
    pin_map_v_.push_back(p);
  }
}
