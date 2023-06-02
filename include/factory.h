#ifndef _VFPGA_FACTORY_H
#define _VFPGA_FACTORY_H

#include <component.h>
#include <jsoncpp/json/json.h>
#include <SDL2/SDL.h>

#include <vector>

class ComponentFactory {
  std::string pic_path_;
  SDL_Renderer *sdl_renderer_;
public:
  ComponentFactory(std::string pic_path, SDL_Renderer *sdl_renderer);
  std::vector<Component *> Manufacture(Json::Value &obj);
  std::vector<SDL_Rect> GetLayout(Json::Value &obj);
  std::vector<PairTexInt> GetTexture(Json::Value &obj);
};

#endif