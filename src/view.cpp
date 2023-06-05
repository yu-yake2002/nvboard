#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <component.h>
#include <jsoncpp/json/json.h>
#include <view.h>
#include <vga.h>

#include <cassert>
#include <iostream>
#include <string>
#include <fstream>

void NVBoardView::load_background(Json::Value obj) {
  for (auto bg : obj) {
    SDL_Surface *surface =
        IMG_Load((this->pic_path + bg["pic"].asString()).c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(this->renderer, surface);
    SDL_Rect rect_bg = {bg["location"][0].asInt(), bg["location"][1].asInt(),
                        bg["shape"][0].asInt(), bg["shape"][1].asInt()};
    SDL_RenderCopy(this->renderer, texture, NULL, &rect_bg);
  }
}

NVBoardView::NVBoardView(std::string board_name) {
  SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  IMG_Init(IMG_INIT_PNG);

  std::string nvboard_home = getenv("NVBOARD_HOME");
  std::ifstream ifs(nvboard_home + "/board/" + board_name + "/config.json");
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj);

  pic_path = std::string(getenv("NVBOARD_HOME")) + "/board/" +
                         obj["name"].asString() + "/pic/";
  int board_width = obj["shape"][0].asInt();
  int board_height = obj["shape"][1].asInt();

  window = SDL_CreateWindow("nvboard", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, board_width,
                                  board_height, SDL_WINDOW_SHOWN);

  Uint32 renderer_flags = 0;
  if (obj.isMember("vsync") && (obj["vsync"].asBool() == true)) {
    renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
  }
  if (obj.isMember("hardware_acc") && obj["hardware_acc"].asBool() == true) {
    renderer_flags |= SDL_RENDERER_ACCELERATED;
  } else {
    renderer_flags |= SDL_RENDERER_SOFTWARE;
  }
  renderer = SDL_CreateRenderer(window, -1, renderer_flags);
  printf("%u\n", renderer_flags);
  load_background(obj["background"]);
  init_components(obj["components"]);
  init_gui();
  UpdateNotRTComponents();
  UpdateRTComponents();
}

void NVBoardView::RendererUpdate() {
  SDL_RenderPresent(this->renderer);
}

NVBoardView::~NVBoardView() {
  for (auto comp_ptr : nrt_components) {
    delete comp_ptr;
  }
  nrt_components.clear();
  for (auto comp_ptr : rt_components) {
    delete comp_ptr;
  }
  rt_components.clear();
  SDL_DestroyWindow(this->window);
  SDL_DestroyRenderer(this->renderer);
}