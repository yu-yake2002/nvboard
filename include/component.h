#ifndef _VFPGA_COMPONENT_H
#define _VFPGA_COMPONENT_H

#include <SDL2/SDL.h>
#include <constrs.h>
#include <factory.h>
#include <jsoncpp/json/json.h>

#include <vector>

// interface type
enum {
  INPUT_TYPE = 1, OUTPUT_TYPE = 2
};

// component type
enum {
  BUTTON_TYPE = 1, SWICTH_TYPE, NAIVE_LED_TYPE, SEGS7_TYPE, VGA_TYPE, KEYBOARD_TYPE
};

union Pin {
  uint16_t m_in;
  uint16_t m_out;
};

class ComponentFactory;

class Component{
protected:
  SDL_Renderer *m_renderer;
  std::vector<SDL_Rect *> m_rects;
  std::vector<SDL_Texture *> m_textures;
  int m_state;
private:
  int m_interface_type;
  int m_component_type;
  std::vector<Pin> pins;

public:
  Component(SDL_Renderer *rend, int cnt, int init_val, int it, int ct);
  ~Component();
  bool in_rect(int x, int y) const;
  int get_interface_type() const;
  int get_component_type() const;
  int get_state() const;
  uint16_t get_input(int idx = 0) const;
  uint16_t get_output(int idx = 0) const;

  void set_rect(SDL_Rect *rect, int val);
  void set_texture(SDL_Texture *texture, int val);
  void add_input(const uint16_t in);
  void add_output(const uint16_t out);
  virtual void update_gui();
  virtual void update_state();
};

class LED : public Component {
 public:
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          Json::Value &obj, ComponentFactory &fac);
};

class Button : public Component {
 public:
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          Json::Value &obj, ComponentFactory &fac);
};

class Switch : public Component {
 public:
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          Json::Value &obj, ComponentFactory &fac);
};

class SEGS7 : public Component {
 public:
  SEGS7(SDL_Renderer *rend, int cnt, int init_val, int it, int ct);
  virtual void update_gui();
  virtual void update_state();
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          Json::Value &obj, ComponentFactory &fac);
};

#endif
