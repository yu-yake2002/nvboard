#ifndef _VFPGA_VGA_H
#define _VFPGA_VGA_H

#include <SDL2/SDL.h>
#include <component.h>
#include <constrs.h>

#define VGA_DEFAULT_WIDTH 640
#define VGA_DEFAULT_HEIGHT 480

namespace NVBoard {

enum {  // VGA_MOD_ID
  VGA_MODE_640_480,
  NR_VGA_MODE
};

struct VGA_MODE {
  int h_frontporch, h_active, h_backporch, h_total;
  int v_frontporch, v_active, v_backporch, v_total;
};

class VGA : public Component {
 private:
  int vga_screen_width, vga_screen_height;
  uint32_t *pixels;
  int vga_pos;
  int vga_pre_clk, vga_pre_vsync, vga_pre_hsync;
  int vga_vaddr, vga_haddr;
  int vga_clk_cnt;

 public:
  VGA(SDL_Renderer *rend, int cnt, int init_val, int it, int ct);
  ~VGA();

  virtual void update_gui();
  virtual void update_state();

  /**
   * The simple factory of VGA.
   *
   * \param renderer the SDL_Renderer you use
   * \param obj the JSON object describing VGA
   * \param fac it should be the caller of this factory
   *
   * \return it return a std::vector<Component *> that contains VGA Components
   */
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          const Json::Value &obj,
                                          const ComponentFactory &fac);
};

}  // namespace NVBoard

#endif
