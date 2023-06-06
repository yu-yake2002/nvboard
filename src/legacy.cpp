/***************************************************************************************
* Copyright (c) 2022 Jiru Sun
* NVBoard is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2. 
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2 
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

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
