#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <component.h>
#include <configs.h>
#include <jsoncpp/json/json.h>
#include <render.h>
#include <vga.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

SDL_Surface *sbutton_on, *sbutton_off;
SDL_Surface *sswitch_on, *sswitch_off;
SDL_Surface *ssegled_ver_off, *ssegled_ver_on,
            *ssegled_hor_off, *ssegled_hor_on,
            *ssegled_dot_off, *ssegled_dot_on;
SDL_Surface *sled_off, *sled_r, *sled_g, *sled_b,
            *sled_rg, *sled_rb, *sled_gb, *sled_rgb;
SDL_Surface *sfpga_background;
SDL_Surface *sseg7_background;

SDL_Texture *tbutton_on, *tbutton_off;
SDL_Texture *tswitch_on, *tswitch_off;
SDL_Texture *tsegled_ver_off, *tsegled_ver_on,
            *tsegled_hor_off, *tsegled_hor_on,
            *tsegled_dot_off, *tsegled_dot_on;
SDL_Texture *tled_off, *tled_r, *tled_g, *tled_b,
            *tled_rg, *tled_rb, *tled_gb, *tled_rgb;

SDL_Texture *segs_texture(int index, int val) {
  switch (index) {
  case 0:
  case 3:
  case 6:
    return val ? tsegled_hor_on : tsegled_hor_off;
    break;
  case 1:
  case 2:
  case 4:
  case 5:
    return val ? tsegled_ver_on : tsegled_ver_off;
    break;
  case 7:
    return val ? tsegled_dot_on : tsegled_dot_off;
    break;
  default:
    assert(0);
    break;
  }
  return nullptr;
}

extern std::string nvboard_home;

void NVBoardRenderer::load_background(Json::Value obj) {
  for (auto bg : obj) {
  SDL_Surface *surface =
      IMG_Load((this->pic_path + bg["pic"].asString()).c_str());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(this->renderer, surface);
  SDL_Rect rect_bg = {bg["location"][0].asInt(), bg["location"][1].asInt(),
                      bg["shape"][0].asInt(), bg["shape"][1].asInt()};
  SDL_RenderCopy(this->renderer, texture, NULL, &rect_bg);
  }
}

static void fill_rect_texture(SDL_Renderer *renderer, SDL_Surface **surface, SDL_Texture **texture, int r, int g, int b) {
  SDL_FillRect(*surface, NULL, SDL_MapRGB((*surface)->format, r, g, b));
  *texture = SDL_CreateTextureFromSurface(renderer, *surface);
}

void NVBoardRenderer::load_texture(Json::Value obj) {
  // buttons
  sbutton_on = IMG_Load((this->pic_path + VBTN_ON_PATH).c_str());
  tbutton_on = SDL_CreateTextureFromSurface(this->renderer, sbutton_on);
  sbutton_off = IMG_Load((this->pic_path + VBTN_OFF_PATH).c_str());
  tbutton_off = SDL_CreateTextureFromSurface(this->renderer, sbutton_off);

  // switches
  sswitch_on = IMG_Load((this->pic_path + VSW_ON_PATH).c_str());
  tswitch_on = SDL_CreateTextureFromSurface(this->renderer, sswitch_on);
  sswitch_off = IMG_Load((this->pic_path + VSW_OFF_PATH).c_str());
  tswitch_off = SDL_CreateTextureFromSurface(this->renderer, sswitch_off);
  
  // 7 segs
  // vertical
  ssegled_ver_on = SDL_CreateRGBSurface(0, SEG_VER_WIDTH, SEG_VER_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &ssegled_ver_on, &tsegled_ver_on, 0xff, 0x00, 0x00);
  ssegled_ver_off = SDL_CreateRGBSurface(0, SEG_VER_WIDTH, SEG_VER_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &ssegled_ver_off, &tsegled_ver_off, 0x2b, 0x2b, 0x2b);

  // horizontal
  ssegled_hor_on = SDL_CreateRGBSurface(0, SEG_HOR_WIDTH, SEG_HOR_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &ssegled_hor_on, &tsegled_hor_on, 0xff, 0x00, 0x00);
  ssegled_hor_off = SDL_CreateRGBSurface(0, SEG_HOR_WIDTH, SEG_HOR_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &ssegled_hor_off, &tsegled_hor_off, 0x2b, 0x2b, 0x2b);
  
  // dot
  ssegled_dot_on = SDL_CreateRGBSurface(0, SEG_DOT_WIDTH, SEG_DOT_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &ssegled_dot_on, &tsegled_dot_on, 0xff, 0x00, 0x00);
  ssegled_dot_off = SDL_CreateRGBSurface(0, SEG_DOT_WIDTH, SEG_DOT_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &ssegled_dot_off, &tsegled_dot_off, 0x2b, 0x2b, 0x2b);
  
  // LEDs
  sled_off = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_off, &tled_off, 0x7f, 0x7f, 0x7f);

  sled_r = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_r, &tled_r, 0xff, 0x00, 0x00);

  sled_g = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_g, &tled_g, 0x00, 0xff, 0x00);
  
  sled_b = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_b, &tled_b, 0x00, 0x00, 0xff);

  sled_rg = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_rg, &tled_rg, 0xff, 0xff, 0x00);

  sled_rb = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_rb, &tled_rb, 0xff, 0x00, 0xff);

  sled_gb = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_gb, &tled_gb, 0x00, 0xff, 0xff);

  sled_rgb = SDL_CreateRGBSurface(0, LED_WIDTH, LED_HEIGHT, 32, 0, 0, 0, 0);
  fill_rect_texture(this->renderer, &sled_rgb, &tled_rgb, 0xff, 0xff, 0xff);
}

NVBoardRenderer::NVBoardRenderer(Json::Value obj) {
  this->pic_path = std::string(getenv("NVBOARD_HOME")) + "/board/" +
                         obj["name"].asString() + "/pic/";
  int board_width = obj["shape"][0].asInt();
  int board_height = obj["shape"][1].asInt();

  this->window = SDL_CreateWindow("nvboard", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, board_width,
                                  board_height, SDL_WINDOW_SHOWN);
  this->renderer = SDL_CreateRenderer(window, -1, 
  #ifdef VSYNC
      SDL_RENDERER_PRESENTVSYNC |
  #endif
  #ifdef HARDWARE_ACC
      SDL_RENDERER_ACCELERATED |
  #else
      SDL_RENDERER_SOFTWARE |
  #endif
      0
  );
  
  load_background(obj["background"]);
  load_texture(obj);
  init_components(obj);
}

SDL_Renderer *NVBoardRenderer::GetRenderer() {
  return this->renderer;
}

void NVBoardRenderer::RendererUpdate() {
  SDL_RenderPresent(this->renderer);
}

NVBoardRenderer::~NVBoardRenderer() {
  SDL_DestroyWindow(this->window);
  SDL_DestroyRenderer(this->renderer);
}