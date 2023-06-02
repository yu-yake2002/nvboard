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

RGB_LED::RGB_LED(SDL_Renderer *rend, int cnt, int init_val, int it, int ct)
    : Component(rend, cnt, init_val, it, ct) {}

void RGB_LED::update_gui() {
  SDL_RenderCopy(get_renderer(), get_texture(get_state()), NULL,
                 get_rect(get_state()));
}

void RGB_LED::update_state() {
  int newval = 0;
  for (int i = 0; i < 3; ++i) {
    newval = (newval << 1) | output_map[get_output(i)];
  }
  if (newval != get_state()) {
    set_state(newval);
    update_gui();
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

void NVBoardViewer::initButton(Json::Value obj) {
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> rect_vec = GetLayout(obj);
  std::vector<PairTexInt> tex_vec = GetTexture(obj);
  
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
    components.push_back(ptr);
  }
}

void NVBoardViewer::initSwitch(Json::Value obj) {
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> rect_vec = GetLayout(obj);
  std::vector<PairTexInt> tex_vec = GetTexture(obj);
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
    components.push_back(ptr);
  }
}

void NVBoardViewer::initLED(Json::Value obj) {
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> rect_vec = GetLayout(obj);
  std::vector<PairTexInt> tex_vec = GetTexture(obj);
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
    components.push_back(ptr);
  }
}

#define GET_SEGA(i) (SEG0A + 8 * i)
#define GET_DECP(i) (SEG0A + 8 * i + 7)

void NVBoardViewer::initSegs7(Json::Value obj) {
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> vec = GetLayout(obj);
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
    components.push_back(ptr);
  }
}

void NVBoardViewer::initVGA(Json::Value obj) {
  Component *ptr = nullptr;
  SDL_Rect *rect_ptr = nullptr;
  std::vector<SDL_Rect> vec = GetLayout(obj);
  int tmp = 0;
  for (auto rect : vec) {
    // init vga
    ptr = new VGA(renderer, 1, 0, OUTPUT_TYPE, VGA_TYPE);
    rect_ptr = new SDL_Rect;
    *rect_ptr = rect;
    ptr->set_rect(rect_ptr, 0);
    for (int p = VGA_VSYNC; p <= VGA_B7; p++) {
      ptr->add_output(p);
    }
    rt_components.push_back(ptr);
  }
  
  extern void vga_set_clk_cycle(int cycle);
  if (obj.isMember("config") && obj["config"].isMember("clk")) {
    vga_set_clk_cycle(obj["config"]["clk"].asInt());
  } else {
    vga_set_clk_cycle(1);
  }
}

void NVBoardViewer::initKeyboard(Json::Value obj) {
  // init keyboard
  extern KEYBOARD *kb;
  kb = new KEYBOARD(renderer, 0, 0, INPUT_TYPE, KEYBOARD_TYPE);
  for (int p = PS2_CLK; p <= PS2_DAT; p++) {
    kb->add_input(p);
  }
  rt_components.push_back(kb);
}

void NVBoardViewer::init_components(Json::Value obj) {
  for (auto i : obj) {
    std::string i_class = i["class"].asString();
    if (i_class == "Button") {
      initButton(i);
    } else if (i_class == "Switch") {
      initSwitch(i);
    } else if (i_class == "LED") {
      initLED(i);
    } else if (i_class == "Segs7") {
      initSegs7(i);
    } else if (i_class == "Keyboard") {
      initKeyboard(i);
    } else if (i_class == "VGA") {
      initVGA(i);
    } else {
      fprintf(stderr, "Component %s is not implemented!\n",
              i["class"].asCString());
    }
  }
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

std::vector<SDL_Rect> NVBoardViewer::GetLayout(Json::Value obj) {
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

std::vector<PairTexInt> NVBoardViewer::GetTexture(Json::Value obj) {
  std::vector<PairTexInt> ret;
  if (obj.isMember("texture")) {
    SDL_Surface *sdl_surface;
    SDL_Texture *sdl_texture;
    Json::Value texture = obj["texture"];
    if (texture["mode"].asString() == "file") {
      for (auto file : texture["file"]) {
        sdl_surface =
            IMG_Load((this->pic_path + file["name"].asString()).c_str());
        sdl_texture = SDL_CreateTextureFromSurface(this->renderer, sdl_surface);
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
        sdl_texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);
        ret.push_back(std::make_pair(sdl_texture, color["val"].asInt()));
      }
    } else {
      fprintf(stderr, "Texture mode %s is not implemented!\n",
              texture["mode"].asCString());
    }
  }
  return ret;
}