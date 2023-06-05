#ifndef VFPGA_RENDER_H
#define VFPGA_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>
#include <list>

class NVBoardView {
private:
  std::list<Component *> nrt_components;
  std::list<Component *> rt_components;  // real-time components
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::string pic_path;
  void load_background(Json::Value obj);
  void init_components(Json::Value obj);
  void init_gui();
  void mousedown_handler(const SDL_Event &ev);
  void mouseup_handler(const SDL_Event &ev);
  void key_handler(uint8_t scancode, int is_keydown);
public:
  void RendererUpdate();
  void UpdateNotRTComponents();
  void UpdateRTComponents();
  int read_event();
  NVBoardView(std::string board_name = "N4");
  ~NVBoardView();
};

#endif
