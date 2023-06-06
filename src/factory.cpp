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

#include <SDL2/SDL.h>
#include <factory.h>
#include <keyboard.h>

namespace NVBoard {

ComponentFactory::ComponentFactory(std::string pic_path,
                                   SDL_Renderer *sdl_renderer)
    : pic_path_(pic_path), sdl_renderer_(sdl_renderer) {}

std::vector<Component *> ComponentFactory::Manufacture(const Json::Value &obj) const {
  std::vector<Component *> ret;
  std::string obj_class = obj["class"].asString();
  if (obj_class == "Button") {
    ret = Button::Factory(sdl_renderer_, obj, *this);
  } else if (obj_class == "Switch") {
    ret = Switch::Factory(sdl_renderer_, obj, *this);
  } else if (obj_class == "LED") {
    ret = LED::Factory(sdl_renderer_, obj, *this);
  } else if (obj_class == "Segs7") {
    ret = SEGS7::Factory(sdl_renderer_, obj, *this);
  } else if (obj_class == "Keyboard") {
    ret = KEYBOARD::Factory(sdl_renderer_, obj, *this);
  } else if (obj_class == "VGA") {
    ret = VGA::Factory(sdl_renderer_, obj, *this);
  } else {
    fprintf(stderr, "Component %s is not implemented!\n",
            obj["class"].asCString());
  }
  return ret;
}

std::vector<SDL_Rect> ComponentFactory::GetLayout(const Json::Value &obj) const {
  std::vector<SDL_Rect> ret;
  if (obj.isMember("layout")) {
    Json::Value layout = obj["layout"];
    int cnt = layout["cnt"].asInt();
    std::pair<int, int> shape =
        std::make_pair(layout["shape"][0].asInt(), layout["shape"][1].asInt());
    if (layout["mode"].asString() == "interval") {
      std::pair<int, int> location = std::make_pair(
          layout["location"][0].asInt(), layout["location"][1].asInt());
      std::pair<int, int> interval = std::make_pair(
          layout["interval"][0].asInt(), layout["interval"][1].asInt());
      for (int i = 0; i < cnt; ++i) {
        ret.push_back((SDL_Rect){location.first + i * interval.first,
                                 location.second + i * interval.second,
                                 shape.first, shape.second});
      }
    } else if (layout["mode"].asString() == "enumerate") {
      if (layout["location"].size() != cnt) {
        fprintf(stderr,
                "Component %s has value %d for the key \"cnt\", "
                "but has %d locations.\n",
                obj["name"].asCString(), cnt, layout["location"].size());
      } else {
        for (int i = 0; i < cnt; ++i) {
          ret.push_back((SDL_Rect){layout["location"][i][0].asInt(),
                                   layout["location"][i][1].asInt(),
                                   shape.first, shape.second});
        }
      }
    } else if (layout["mode"].asString() == "null") {
      for (int i = 0; i < cnt; ++i) {
        ret.push_back((SDL_Rect){-1, -1, -1, -1});
      }
    } else {
      fprintf(stderr, "Layout mode %s is not implemented!\n",
              layout["mode"].asCString());
    }
  }
  return ret;
}

std::vector<PairTexInt> ComponentFactory::GetTexture(const Json::Value &obj) const {
  std::vector<PairTexInt> ret;
  if (obj.isMember("texture")) {
    SDL_Surface *sdl_surface;
    SDL_Texture *sdl_texture;
    Json::Value texture = obj["texture"];
    if (texture["mode"].asString() == "file") {
      for (auto file : texture["file"]) {
        sdl_surface = IMG_Load((pic_path_ + file["name"].asString()).c_str());
        sdl_texture = SDL_CreateTextureFromSurface(sdl_renderer_, sdl_surface);
        ret.push_back(std::make_pair(sdl_texture, file["val"].asInt()));
      }
    } else if (texture["mode"].asString() == "color") {
      int width = obj["layout"]["shape"][0].asInt();
      int height = obj["layout"]["shape"][0].asInt();
      for (auto color : texture["color"]) {
        sdl_surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        Json::Value rgb = color["rgb"];
        SDL_FillRect(sdl_surface, NULL,
                     SDL_MapRGB(sdl_surface->format, rgb[0].asInt(),
                                rgb[1].asInt(), rgb[2].asInt()));
        sdl_texture = SDL_CreateTextureFromSurface(sdl_renderer_, sdl_surface);
        ret.push_back(std::make_pair(sdl_texture, color["val"].asInt()));
      }
    } else {
      fprintf(stderr, "Texture mode %s is not implemented!\n",
              texture["mode"].asCString());
    }
  }
  return ret;
}

}  // namespace NVBoard
