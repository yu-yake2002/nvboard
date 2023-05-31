#ifndef VFPGA_RENDER_H
#define VFPGA_RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>

// TODO: remove me!
#define WINDOW_WIDTH   640

#define SWITCH_X       60
#define SWITCH_Y       400
#define SWITCH_SEP     10
#define SWITCH_WIDTH   20
#define SWITCH_HEIGHT  40

#define BTNC_X         520
#define BTNC_Y         250
#define BTNC_SEP       20
#define BTNC_WIDTH     30
#define BTNC_HEIGHT    30

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
  SDL_Window *main_window;
  SDL_Renderer *main_renderer;
  std::string pic_path;
  std::vector<SDL_Surface *> surfaces;
  std::vector<SDL_Texture *> textures;
  void load_background(Json::Value obj);
  void load_texture(Json::Value obj);

public:
  void RendererUpdate();
  SDL_Renderer *GetRenderer();
  NVBoardRenderer(Json::Value obj);
  ~NVBoardRenderer();
};

#endif
