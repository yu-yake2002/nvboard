#ifndef _VFPGA_KEYBOARD_H
#define _VFPGA_KEYBOARD_H

#include <assert.h>
#include <at_scancode.h>
#include <nvboard.h>

#include <map>
#include <queue>

#define NOT_A_KEY -1
#define CLK_NUM 10
#define PS2_START 0
#define PS2_DATA_0 1
#define PS2_DATA_7 8
#define PS2_PARTIAL 9
#define PS2_STOP 10

#define UINT2_XOR(a) (((a) >> 1) ^ ((a)&0b1))
#define UINT4_XOR(a) UINT2_XOR(((a) >> 2) ^ ((a)&0b11))
#define UINT8_XOR(a) UINT4_XOR(((a) >> 4) ^ ((a)&0b1111))

namespace NVBoard {

class KEYBOARD : public Component {
 private:
  std::queue<uint8_t> all_keys_;
  int data_idx_;
  int left_clk_;
  int cur_key_;
  uint64_t *ps2_clk_;
  uint64_t *ps2_dat_;

 public:
  KEYBOARD(SDL_Renderer *rend, int cnt, int init_val, int it, int ct);

  /**
   * Push a scancode to the keyboard's output queue.
   *
   * \param scancode see at_scancode.h
   * \param is_keydown true for down and false for up
   */
  void push_key(uint8_t scancode, bool is_keydown);

  virtual void update_state();

  /**
   * The simple factory of Keyboard.
   *
   * \param renderer the SDL_Renderer you use
   * \param obj the JSON object describing Keyboard
   * \param fac it should be the caller of this factory
   *
   * \return it return a std::vector<Component *> that contains Keyboard
   * Components
   */
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          const Json::Value &obj,
                                          const ComponentFactory &fac);
};

}  // namespace NVBoard

#endif
