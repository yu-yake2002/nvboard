#include <legacy.h>
#include <nvboard.h>

static NVBoard::NVBoardController *controller = nullptr;

void nvboard_init() {
  if (controller == nullptr) {
    controller = new NVBoard::NVBoardController();
  }
}

void nvboard_quit() {
  delete controller;
}

void nvboard_bind_pin(void *signal, bool is_rt, bool is_output, int len, ...) {
  if (controller == nullptr) {
    controller = new NVBoard::NVBoardController();
  }
  std::list<int> pins;
  va_list ap;
  va_start(ap, len);
  for (int i = 0; i < len; ++i) {
    pins.push_back(va_arg(ap, int));
  }
  controller->NVBoardBindPin(signal, is_rt, is_output, len, pins);
}

void nvboard_update() {
  controller->NVBoardUpdate();
}
