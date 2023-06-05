#ifndef _VFPGA_FACTORY_H
#define _VFPGA_FACTORY_H

#include <SDL2/SDL.h>
#include <component.h>
#include <jsoncpp/json/json.h>

#include <vector>

namespace NVBoard {

typedef std::pair<SDL_Texture *, int> PairTexInt;
class Component;

class ComponentFactory {
  std::string pic_path_;
  SDL_Renderer *sdl_renderer_;

 public:
  ComponentFactory(std::string pic_path, SDL_Renderer *sdl_renderer);

  /**
   * Simple factory method of Components.
   *
   * \param obj a JSON object describing the Components
   * \return it return a std::vector<Component *> that contains right Components
   */
  std::vector<Component *> Manufacture(Json::Value &obj);

  std::vector<SDL_Rect> GetLayout(Json::Value &obj);
  std::vector<PairTexInt> GetTexture(Json::Value &obj);
};

}  // namespace NVBoard

#endif
