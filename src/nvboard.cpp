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

namespace NVBoard {

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
  void *ptr = p->signal_;
  if (p->len_ == 1) {
    uint8_t val = input_map[p->pin_];
    *(uint8_t *)ptr = val;
    return;
  }

  int len = p->len_;
  uint64_t val = 0;
  for (int i = 0; i < len; i++) {
    val <<= 1;
    val |= input_map[p->pins_[i]];
  }
  if (len <= 8) {
    *(uint8_t *)ptr = val;
  } else if (len <= 16) {
    *(uint16_t *)ptr = val;
  } else if (len <= 32) {
    *(uint32_t *)ptr = val;
  } else if (len <= 64) {
    *(uint64_t *)ptr = val;
  }
}

void NVBoardController::UpdateOutputPin(PinMap *p) {
  void *ptr = p->signal_;
  if (p->len_ == 1) {
    uint8_t val = *(uint8_t *)ptr;
    output_map[p->pin_] = val & 1;
    return;
  }

  int len = p->len_;
  uint64_t val = 0;
  if (len <= 8) {
    val = *(uint8_t *)ptr;
  } else if (len <= 16) {
    val = *(uint16_t *)ptr;
  } else if (len <= 32) {
    val = *(uint32_t *)ptr;
  } else if (len <= 64) {
    val = *(uint64_t *)ptr;
  }
  for (int i = 0; i < len; i++) {
    output_map[p->pins_[i]] = val & 1;
    val >>= 1;
  }
}

int NVBoardController::NVBoardUpdate() {
  for (auto p : rt_pin_map_v_) {
    if (p->is_output_)
      UpdateOutputPin(p);
    else
      UpdateInputPin(p);
  }

  viewer_->UpdateRTComponents();

  int ret = 1;
  static uint64_t last = 0;
  uint64_t now = get_time();
  if (now - last > 1000000 / FPS) {
    last = now;

    for (auto p : pin_map_v_) {
      if (p->is_output_)
        UpdateOutputPin(p);
      else
        UpdateInputPin(p);
    }

    if (viewer_->read_event() == -1) {
      ret = 0;
    }

    viewer_->UpdateNotRTComponents();
    viewer_->RendererUpdate();
  }
  return ret;
}

NVBoardController::NVBoardController(std::string board) {
  printf("NVBoard v0.2\n");
  viewer_ = new NVBoardView(board);
  boot_time = get_time_internal();
}

NVBoardController::~NVBoardController() {
  for (auto p : pin_map_v_) {
    delete p;
  }
  for (auto p : rt_pin_map_v_) {
    delete p;
  }
  delete viewer_;
  IMG_Quit();
  SDL_Quit();
}

void NVBoardController::NVBoardBindPin(void *signal, bool is_rt, bool is_output,
                                       int len, ...) {
  PinMap *p = nullptr;
  assert(len < 64);

  va_list ap;
  va_start(ap, len);
  if (len == 1) {
    p = new PinMap(signal, is_output, len, (uint16_t)va_arg(ap, int));
  } else {
    uint16_t *pins = new uint16_t[len];
    for (int i = 0; i < len; i++) {
      uint16_t pin = va_arg(ap, int);
      if (is_output)
        pins[len - 1 - i] = pin;
      else
        pins[i] = pin;
    }
    p = new PinMap(signal, is_output, len, pins);
    delete[] pins;
  }
  va_end(ap);

  if (is_rt) {
    rt_pin_map_v_.push_back(p);
  } else {
    pin_map_v_.push_back(p);
  }
}

void NVBoardController::NVBoardBindPin(void *signal, bool is_rt, bool is_output, int len, const std::list<int> &pins_list) {
  PinMap *p = nullptr;
  assert(len < 64);

  if (len == 1) {
    p = new PinMap(signal, is_output, len, pins_list.front());
  } else {
    uint16_t *pins = new uint16_t[len];
    int i = 0;
    for (auto pin : pins_list) {
      if (is_output)
        pins[len - 1 - i] = pin;
      else
        pins[i] = pin;
      i++;
    }
    p = new PinMap(signal, is_output, len, pins);
    delete[] pins;
  }

  if (is_rt) {
    rt_pin_map_v_.push_back(p);
  } else {
    pin_map_v_.push_back(p);
  }
}

PinMap::PinMap(void *signal, bool is_output, int len, uint16_t pin)
    : is_output_(is_output), len_(len), signal_(signal), pin_(pin) {
  assert(len == 1);
}

PinMap::PinMap(void *signal, bool is_output, int len, uint16_t *pins)
    : is_output_(is_output),
      len_(len),
      signal_(signal),
      pins_(new uint16_t[len]) {
  assert(len != 1 && len < 64);
  memcpy(pins_, pins, len * sizeof(uint16_t));
}

PinMap::~PinMap() {
  if (len_ != 1) {
    delete[] pins_;
  }
}

}  // namespace NVBoard
