#ifndef VFPGA_RENDER_H
#define VFPGA_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>

#define LED_X          60 + (4/2)
#define LED_Y          360
#define LED_SEP        14
#define LED_WIDTH      16
#define LED_HEIGHT     8

#define SEG_X          60
#define SEG_Y          225

#define SEG_VER_WIDTH  3
#define SEG_VER_HEIGHT 30
#define SEG_HOR_WIDTH  30
#define SEG_HOR_HEIGHT 3
#define SEG_DOT_WIDTH  4
#define SEG_DOT_HEIGHT 4

#define SEG_SEP        3

#define SEG_TOT_WIDTH  (SEG_SEP * 18 + SEG_VER_WIDTH * 16 + SEG_DOT_WIDTH * 8 + SEG_HOR_WIDTH * 8)
#define SEG_TOT_HEIGHT (SEG_SEP * 4 + SEG_VER_HEIGHT * 2 + SEG_HOR_HEIGHT * 3)

class NVBoardRenderer {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::string pic_path;
  std::vector<SDL_Surface *> surfaces;
  std::vector<SDL_Texture *> textures;
  void load_background(Json::Value obj);
  void load_texture(Json::Value obj);
  void init_components(Json::Value obj);
  void initButton(Json::Value obj);
  void initSwitch(Json::Value obj);
  void initLED(Json::Value obj);
  void initSegs7(Json::Value obj);
  void initVGA(Json::Value obj);
  void initKeyboard(Json::Value obj);
public:
  void RendererUpdate();
  SDL_Renderer *GetRenderer();
  NVBoardRenderer(Json::Value obj);
  ~NVBoardRenderer();
};

#endif
