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

#ifndef VFPGA_RENDER_H
#define VFPGA_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>

#include <list>

namespace NVBoard {

class NVBoardView {
 private:
  std::list<Component *> nrt_components_;
  std::list<Component *> rt_components_;  // real-time components
  SDL_Window *window_;
  SDL_Renderer *renderer_;
  std::string pic_path_;
  void load_background(Json::Value &obj);
  void init_components(Json::Value &obj);

  /**
   * Init GUI. Render buttons, switches, leds and 7-segs
   */
  void init_gui();

  void mousedown_handler(const SDL_Event &ev);
  void mouseup_handler(const SDL_Event &ev);
  void key_handler(uint8_t scancode, int is_keydown);

 public:
  /**
   * Call SDL_RenderPresent to refresh the window
   */
  void RendererUpdate();

  /**
   * Update the state of non-real-time components, such as LED, switch, and
   * button. Their states don't need updating in every clock cycle.
   */
  void UpdateNotRTComponents();

  /**
   * Update the state of real-time components, such as VGA and PS/2. Their
   * states need updating in every clock cycle.
   */
  void UpdateRTComponents();

  /**
   * Read SDL event.
   *
   * \return Return -1 when esc is pressed. Else check whether buttons /
   * switches are pressed, deal with the event, and then return 0.
   */
  int read_event();
  NVBoardView(std::string board_name = "N4");
  ~NVBoardView();
};

}  // namespace NVBoard

#endif
