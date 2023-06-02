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
  init_components(obj["components"]);
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