#ifndef VFPGA_RENDER_H
#define VFPGA_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>
#include <list>

typedef std::pair<SDL_Texture *, int> PairTexInt;

class NVBoardViewer {
private:
  std::list<Component *> components;
  std::list<Component *> rt_components;  // real-time components
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::string pic_path;
  std::vector<SDL_Surface *> surfaces;
  std::vector<SDL_Texture *> textures;
  void load_background(Json::Value obj);
  void init_components(Json::Value obj);
  void init_gui();
  void mousedown_handler(const SDL_Event &ev);
  void mouseup_handler(const SDL_Event &ev);
  void key_handler(uint8_t scancode, int is_keydown);
public:
  void RendererUpdate();
  SDL_Renderer *GetRenderer();
  void UpdateNotRTComponents();
  void UpdateRTComponents();
  void delete_components();
  int read_event();
  NVBoardViewer(std::string board_name = "N4");
  ~NVBoardViewer();
};

#endif
