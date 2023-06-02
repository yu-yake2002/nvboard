#ifndef VFPGA_RENDER_H
#define VFPGA_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>

typedef std::pair<SDL_Texture *, int> PairTexInt;

class NVBoardRenderer {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::string pic_path;
  std::vector<SDL_Surface *> surfaces;
  std::vector<SDL_Texture *> textures;
  void load_background(Json::Value obj);
  void init_components(Json::Value obj);
  void initButton(Json::Value obj);
  void initSwitch(Json::Value obj);
  void initLED(Json::Value obj);
  void initSegs7(Json::Value obj);
  void initVGA(Json::Value obj);
  void initKeyboard(Json::Value obj);
  std::vector<SDL_Rect> GetLayout(Json::Value obj);
  std::vector<PairTexInt> GetTexture(Json::Value obj);
public:
  void RendererUpdate();
  SDL_Renderer *GetRenderer();
  NVBoardRenderer(Json::Value obj);
  ~NVBoardRenderer();
};

#endif
