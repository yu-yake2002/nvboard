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

#include <assert.h>
#include <keyboard.h>
#include <macro.h>
#include <nvboard.h>

#define FILL_KEYMAP_FIRST(a) \
  keymap_first[SDL_PREFIX(a)] = GET_FIRST(AT_PREFIX(a));
#define FILL_KEYMAP_DECOND(a) \
  keymap_second[SDL_PREFIX(a)] = GET_SECOND(AT_PREFIX(a));

namespace NVBoard {

KEYBOARD *kb;

static int keymap_first[256] = {};
static int keymap_second[256] = {};

static void keymap_init() {
  MAP(SCANCODE_LIST, FILL_KEYMAP_FIRST)
  MAP(SCANCODE_LIST, FILL_KEYMAP_DECOND)
}

static int sdl2at(int scancode, int is_first) {
  return is_first ? keymap_first[scancode] : keymap_second[scancode];
}

KEYBOARD::KEYBOARD(SDL_Renderer *rend, int cnt, int init_val, int it, int ct)
    : Component(rend, cnt, init_val, it, ct),
      data_idx_(0),
      left_clk_(0),
      cur_key_(NOT_A_KEY),
      ps2_clk_(&input_map[PS2_CLK]),
      ps2_dat_(&input_map[PS2_DAT]) {
  keymap_init();
}

void KEYBOARD::push_key(uint8_t sdl_key, bool is_keydown) {
  uint8_t at_key = sdl2at(sdl_key, 1);
  if (at_key == 0xe0) {
    all_keys_.push(0xe0);
    at_key = sdl2at(sdl_key, 0);
  }
  if (!is_keydown) all_keys_.push(0xf0);
  all_keys_.push(at_key);
}

void KEYBOARD::update_state() {
  if (cur_key_ == NOT_A_KEY) {
    if (all_keys_.empty()) return;
    cur_key_ = all_keys_.front();
    assert(data_idx_ == 0);
    left_clk_ = CLK_NUM;
    *ps2_clk_ == 1;
  }

  if (left_clk_ == 0) {
    *ps2_clk_ = !*ps2_clk_;
    left_clk_ = CLK_NUM;
    if (*ps2_clk_) {
      assert(!all_keys_.empty());
      *ps2_dat_ = (data_idx_ == PS2_PARTIAL) ? !UINT8_XOR(all_keys_.front())
                  : (data_idx_ == PS2_STOP)  ? 1
                  : ((data_idx_ >= PS2_DATA_0) && (data_idx_ <= PS2_DATA_7))
                      ? (cur_key_ & 1)
                      : 0;
      if ((data_idx_ >= PS2_DATA_0) && (data_idx_ <= PS2_DATA_7))
        cur_key_ >>= 1;
      data_idx_++;
    } else if (data_idx_ == 11) {
      data_idx_ = 0;
      cur_key_ = NOT_A_KEY;
      all_keys_.pop();
    }
  } else {
    left_clk_--;
  }
}

std::vector<Component *> KEYBOARD::Factory(SDL_Renderer *renderer,
                                           const Json::Value &obj,
                                           const ComponentFactory &fac) {
  std::vector<Component *> ret;
  extern KEYBOARD *kb;
  kb = new KEYBOARD(renderer, 0, 0, INPUT_TYPE, KEYBOARD_TYPE);
  for (int p = PS2_CLK; p <= PS2_DAT; p++) {
    kb->add_input(p);
  }
  ret.push_back(kb);
  return ret;
}

}  // namespace NVBoard
