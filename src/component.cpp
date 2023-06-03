#include <SDL2/SDL.h>
#include <nvboard.h>

#include <iostream>
#include <map>
#include <vector>
#include <list>

extern uint64_t input_map[];
extern uint64_t output_map[];

Component::Component(SDL_Renderer *rend, int cnt, int init_val, int it,
                     int ct) {
  m_renderer = rend;
  m_interface_type = it;
  m_component_type = ct;
  m_rects.resize(cnt);
  m_textures.resize(cnt);
  m_state = init_val;
}

bool Component::in_rect(int x, int y) const {
  SDL_Rect *temp = m_rects[0];
  return x >= temp->x && y >= temp->y && x < temp->x + temp->w &&
         y < temp->y + temp->h;
}

SDL_Renderer *Component::get_renderer() const { return m_renderer; }

int Component::get_interface_type() const { return m_interface_type; }

int Component::get_component_type() const { return m_component_type; }

SDL_Rect *Component::get_rect(int idx) const { return m_rects[idx]; }

SDL_Texture *Component::get_texture(int idx) const { return m_textures[idx]; }

int Component::get_state() const { return m_state; }

uint16_t Component::get_input(int idx) const { return pins[idx].m_in; }

uint16_t Component::get_output(int idx) const { return pins[idx].m_out; }

void Component::set_rect(SDL_Rect *rect, int val) { m_rects[val] = rect; }

void Component::set_texture(SDL_Texture *texture, int val) {
  m_textures[val] = texture;
}

void Component::set_state(int val) { m_state = val; }

void Component::add_input(const uint16_t in) {
  Pin temp;
  temp.m_in = in;
  pins.push_back(temp);
}

void Component::add_output(const uint16_t out) {
  Pin temp;
  temp.m_out = out;
  pins.push_back(temp);
}

void Component::update_gui() {
  SDL_RenderCopy(m_renderer, m_textures[m_state], NULL, m_rects[m_state]);
}

void Component::update_state() {
  Pin pin = *(pins.begin());
  int newval = (m_interface_type == INPUT_TYPE) ? input_map[pin.m_in]
                                                : output_map[pin.m_out];
  if (newval != m_state) {
    set_state(newval);
    update_gui();
  }
}

void Component::remove() {
  for (auto rect_ptr : m_rects) {
    delete rect_ptr;
  }
}

SEGS7::SEGS7(SDL_Renderer *rend, int cnt, int init_val, int it, int ct)
    : Component(rend, cnt, init_val, it, ct) {}

void SEGS7::update_gui() {
  int newval = get_state();
  for (int i = 0; i < 16; ++i) {
    if ((newval >> i) & 1) {
      SDL_RenderCopy(get_renderer(), get_texture(i), NULL, get_rect(i));
    }
  }
}

void SEGS7::update_state() {
  int newval = 0;
  for (int i = 0; i < 8; ++i) {
    newval |=
        (output_map[get_output(i)]) ? (1 << (i << 1)) : (1 << (i << 1 | 1));
  }
  if (newval != get_state()) {
    set_state(newval);
    update_gui();
  }
}

extern SDL_Texture *tsegled_ver_off, *tsegled_ver_on, *tsegled_hor_off,
    *tsegled_hor_on, *tsegled_dot_off, *tsegled_dot_on;


SDL_Rect operator+(const SDL_Rect &A, const SDL_Rect &B) {
  SDL_Rect ret;
  ret.x = A.x + B.x;
  ret.y = A.y + B.y;
  ret.w = A.w + B.w;
  ret.h = A.h + B.h;
  return ret;
}

void NVBoardViewer::init_components(Json::Value obj) {
  ComponentFactory *factory = new ComponentFactory(pic_path, renderer);
  for (auto i : obj) {
    auto vec = factory->Manufacture(i);
    if (!i.isMember("rt") || (i["rt"].asBool() == true)) {
      rt_components.insert(rt_components.end(), vec.begin(), vec.end());
    } else {
      components.insert(components.end(), vec.begin(), vec.end());
    }
  }
  delete factory;
}

void NVBoardViewer::delete_components() {
  for (auto comp_ptr : components) {
    comp_ptr->remove();
    delete comp_ptr;
  }
  components.clear();
  for (auto comp_ptr : rt_components) {
    comp_ptr->remove();
    delete comp_ptr;
  }
  rt_components.clear();
}

// render buttons, switches, leds and 7-segs
void NVBoardViewer::init_gui() {
  for (auto ptr : components) {
    ptr->update_gui();
  }
}

void NVBoardViewer::UpdateNotRTComponents() {
  for (auto ptr : components) {
    ptr->update_state();
  }
}

void NVBoardViewer::UpdateRTComponents() {
  for (auto ptr : rt_components) {
    ptr->update_state();
  }
}

std::vector<Component *> Button::Factory(SDL_Renderer *renderer, Json::Value &obj, ComponentFactory &fac) {
  std::vector<Component *> ret;
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> rect_vec = fac.GetLayout(obj);
  std::vector<PairTexInt> tex_vec = fac.GetTexture(obj);
  int tmp = 0;
  for (auto rect : rect_vec) {
    ptr = new Component(renderer, 2, 0, INPUT_TYPE, BUTTON_TYPE);

    for (auto tex : tex_vec) {
      rect_ptr = new SDL_Rect;
      *rect_ptr = rect;
      ptr->set_rect(rect_ptr, tex.second);
      ptr->set_texture(tex.first, tex.second);
    }

    ptr->add_input(BTNC + tmp);
    ++tmp;
    ret.push_back(ptr);
  }
  return ret;
}

std::vector<Component *> Switch::Factory(SDL_Renderer *renderer, Json::Value &obj, ComponentFactory &fac) {
  std::vector<Component *> ret;
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> rect_vec = fac.GetLayout(obj);
  std::vector<PairTexInt> tex_vec = fac.GetTexture(obj);
  int tmp = 0;
  for (auto rect : rect_vec) {
    ptr = new Component(renderer, 2, 0, INPUT_TYPE, SWICTH_TYPE);
    
    for (auto tex : tex_vec) {
      rect_ptr = new SDL_Rect;
      *rect_ptr = rect;
      ptr->set_rect(rect_ptr, tex.second);
      ptr->set_texture(tex.first, tex.second);
    }

    ptr->add_input(SW0 + tmp);
    ++tmp;
    ret.push_back(ptr);
  }
  return ret;
}

std::vector<Component *> LED::Factory(SDL_Renderer *renderer, Json::Value &obj, ComponentFactory &fac) {
  std::vector<Component *> ret;
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> rect_vec = fac.GetLayout(obj);
  std::vector<PairTexInt> tex_vec = fac.GetTexture(obj);
  int tmp = 0;
  // init naive leds
  for (auto rect : rect_vec) {
    ptr = new Component(renderer, 2, 0, OUTPUT_TYPE, NAIVE_LED_TYPE);
    
    for (auto tex : tex_vec) {
      rect_ptr = new SDL_Rect;
      *rect_ptr = rect;
      ptr->set_rect(rect_ptr, tex.second);
      ptr->set_texture(tex.first, tex.second);
    }

    ptr->add_output(LD0 + tmp);
    ++tmp;
    ret.push_back(ptr);
  }
  return ret;
}

#define GET_SEGA(i) (SEG0A + 8 * i)
#define GET_DECP(i) (SEG0A + 8 * i + 7)

std::vector<Component *> SEGS7::Factory(SDL_Renderer *renderer, Json::Value &obj, ComponentFactory &fac) {
  std::vector<Component *> ret;
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> vec = fac.GetLayout(obj);
  int tmp = 0;
  Json::Value config = obj["config"];
  Json::Value texture = obj["texture"];
  for (auto rect : vec) {
    ptr = new SEGS7(renderer, 16, 0x5555, OUTPUT_TYPE, SEGS7_TYPE);
    for (int j = 0; j < 8; ++j) {
      SDL_Surface *sdl_surface;
      SDL_Texture *sdl_texture;
      SDL_Rect seg_rect = (SDL_Rect){
          config["location"][j][0].asInt(), config["location"][j][1].asInt(),
          config["shape"][config["type"][j].asString()][0].asInt(),
          config["shape"][config["type"][j].asString()][1].asInt()};

      assert(texture["mode"].asString() == "color");
      for (auto color : texture["color"]) {
        Json::Value rgb = color["rgb"];
        rect_ptr = new SDL_Rect;
        *rect_ptr = rect + seg_rect;
        sdl_surface =
            SDL_CreateRGBSurface(0, rect_ptr->w, rect_ptr->h, 32, 0, 0, 0, 0);
        SDL_FillRect(sdl_surface, NULL,
                     SDL_MapRGB(sdl_surface->format, rgb[0].asInt(),
                                rgb[1].asInt(), rgb[2].asInt()));
        sdl_texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);
        ptr->set_texture(sdl_texture, j << 1 | color["val"].asInt());
        ptr->set_rect(rect_ptr, j << 1 | color["val"].asInt());
      }
    }

    for (int p = GET_SEGA(tmp); p <= GET_DECP(tmp); p++) {
      ptr->add_output(p);
    }
    ++tmp;
    ret.push_back(ptr);
  }
  return ret;
}
