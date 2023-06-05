#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <component.h>
#include <jsoncpp/json/json.h>
#include <vga.h>
#include <view.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

namespace NVBoard {

void NVBoardView::load_background(Json::Value &obj) {
  for (auto bg : obj) {
    SDL_Surface *surface = IMG_Load((pic_path_ + bg["pic"].asString()).c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_Rect rect_bg = {bg["location"][0].asInt(), bg["location"][1].asInt(),
                        bg["shape"][0].asInt(), bg["shape"][1].asInt()};
    SDL_RenderCopy(renderer_, texture, NULL, &rect_bg);
  }
}

void NVBoardView::init_components(Json::Value &obj) {
  ComponentFactory *factory = new ComponentFactory(pic_path_, renderer_);
  for (auto i : obj) {
    auto vec = factory->Manufacture(i);
    if (!i.isMember("rt") || (i["rt"].asBool() == true)) {
      rt_components_.insert(rt_components_.end(), vec.begin(), vec.end());
    } else {
      nrt_components_.insert(nrt_components_.end(), vec.begin(), vec.end());
    }
  }
  delete factory;
}

void NVBoardView::init_gui() {
  for (auto ptr : nrt_components_) {
    ptr->update_gui();
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
  if (obj.isMember("version")) {
    printf("Loading board %s %s\n", obj["name"].asCString(), obj["version"].asCString());
  } else {
    printf("Loading board %s\n", obj["name"].asCString());
  }
  pic_path_ = std::string(getenv("NVBOARD_HOME")) + "/board/" +
              obj["name"].asString() + "/pic/";
  int board_width = obj["shape"][0].asInt();
  int board_height = obj["shape"][1].asInt();

  window_ = SDL_CreateWindow("nvboard", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, board_width, board_height,
                             SDL_WINDOW_SHOWN);

  Uint32 renderer_flags = 0;
  if (obj.isMember("vsync") && (obj["vsync"].asBool() == true)) {
    renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
  }
  if (obj.isMember("hardware_acc") && obj["hardware_acc"].asBool() == true) {
    renderer_flags |= SDL_RENDERER_ACCELERATED;
  } else {
    renderer_flags |= SDL_RENDERER_SOFTWARE;
  }
  renderer_ = SDL_CreateRenderer(window_, -1, renderer_flags);

  load_background(obj["background"]);
  init_components(obj["components"]);
  init_gui();
  UpdateNotRTComponents();
  UpdateRTComponents();
}

void NVBoardView::RendererUpdate() { SDL_RenderPresent(renderer_); }

NVBoardView::~NVBoardView() {
  for (auto comp_ptr : nrt_components_) {
    delete comp_ptr;
  }
  nrt_components_.clear();
  for (auto comp_ptr : rt_components_) {
    delete comp_ptr;
  }
  rt_components_.clear();
  SDL_DestroyWindow(window_);
  SDL_DestroyRenderer(renderer_);
}

void NVBoardView::UpdateNotRTComponents() {
  for (auto ptr : nrt_components_) {
    ptr->update_state();
  }
}

void NVBoardView::UpdateRTComponents() {
  for (auto ptr : rt_components_) {
    ptr->update_state();
  }
}

}  // namespace NVBoard
