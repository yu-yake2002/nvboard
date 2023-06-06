/***************************************************************************************
* Copyright (c) 2022 Jiru Sun
* NVBoard is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2. 
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2 
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

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
  std::vector<Component *> Manufacture(const Json::Value &obj) const;

  std::vector<SDL_Rect> GetLayout(const Json::Value &obj) const;
  std::vector<PairTexInt> GetTexture(const Json::Value &obj) const;
};

}  // namespace NVBoard

#endif
