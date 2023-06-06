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

#ifndef _VFPGA_COMPONENT_H
#define _VFPGA_COMPONENT_H

#include <SDL2/SDL.h>
#include <constrs.h>
#include <factory.h>
#include <jsoncpp/json/json.h>

#include <vector>

namespace NVBoard {

// interface type
enum { INPUT_TYPE = 1, OUTPUT_TYPE = 2 };

// component type
enum {
  BUTTON_TYPE = 1,
  SWICTH_TYPE,
  NAIVE_LED_TYPE,
  SEGS7_TYPE,
  VGA_TYPE,
  KEYBOARD_TYPE
};

union Pin {
  uint16_t m_in;
  uint16_t m_out;
};

class ComponentFactory;

class Component {
 protected:
  SDL_Renderer *renderer_;
  std::vector<SDL_Rect *> rects_;
  std::vector<SDL_Texture *> textures_;
  int state_;

 private:
  int interface_type_;
  int component_type_;
  std::vector<Pin> pins_;

 public:
  Component(SDL_Renderer *rend, int cnt, int init_val, int it, int ct);
  ~Component();

  /**
   * Check whether the point is in the Component's rect[0].
   *
   * \param x horizontal position of the point
   * \param y vertical position of the point
   */
  bool in_rect(int x, int y) const;

  /**
   * Get the Component's interface_type.
  */
  int get_interface_type() const;

  /**
   * Get the Component's component_type.
  */
  int get_component_type() const;

  /**
   * Get the Component's state.
  */
  int get_state() const;

  /**
   * Read the Component's input pin.
   *
   * \param idx the index of pin in this component. You are reading
   * pins_[idx].m_in
   */
  uint16_t get_input(int idx = 0) const;

  /**
   * Read the Component's output pin.
   *
   * \param idx the index of pin in this component. You are reading
   * pins_[idx].m_out
   */
  uint16_t get_output(int idx = 0) const;
  
  /**
   * Set the rect of component.
   * 
   * \param rect
   * \param idx the index of rect in this component
  */
  void set_rect(SDL_Rect *rect, int idx);
  
  /**
   * Set the texture of component.
   * 
   * \param texture
   * \param idx the index of rect in this component
  */
  void set_texture(SDL_Texture *texture, int idx);

  /**
   * Add an input pin to the component.
   * 
   * \param in input pin's index in NVBoard
  */
  void add_input(const uint16_t in);
  
  /**
   * Add an output pin to the component.
   * 
   * \param out output pin's index in NVBoard
  */
  void add_output(const uint16_t out);

  /**
   * Call SDL_RenderCopy to update texture. Note that it doesn't refresh the
   * window.
   */
  virtual void update_gui();

  /**
   * Update the component's state. If the state changes, call update_gui.
   */
  virtual void update_state();
};

class LED : public Component {
 public:
  /**
   * The simple factory of LED.
   * 
   * \param renderer the SDL_Renderer you use
   * \param obj the JSON object describing LEDs
   * \param fac it should be the caller of this factory
   * 
   * \return it return a std::vector<Component *> that contains LED Components
  */
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          const Json::Value &obj,
                                          const ComponentFactory &fac);
};

class Button : public Component {
 public:
  /**
   * The simple factory of Button.
   *
   * \param renderer the SDL_Renderer you use
   * \param obj the JSON object describing Buttons
   * \param fac it should be the caller of this factory
   *
   * \return it return a std::vector<Component *> that contains Button
   * Components
   */
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          const Json::Value &obj,
                                          const ComponentFactory &fac);
};

class Switch : public Component {
 public:
  /**
   * The simple factory of Switch.
   *
   * \param renderer the SDL_Renderer you use
   * \param obj the JSON object describing Switchs
   * \param fac it should be the caller of this factory
   *
   * \return it return a std::vector<Component *> that contains Switch
   * Components
   */
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          const Json::Value &obj,
                                          const ComponentFactory &fac);
};

class SEGS7 : public Component {
 public:
  SEGS7(SDL_Renderer *rend, int cnt, int init_val, int it, int ct);
  virtual void update_gui();
  virtual void update_state();
  /**
   * The simple factory of SEGS7.
   *
   * \param renderer the SDL_Renderer you use
   * \param obj the JSON object describing SEGS7
   * \param fac it should be the caller of this factory
   *
   * \return it return a std::vector<Component *> that contains SEGS7
   * Components
   */
  static std::vector<Component *> Factory(SDL_Renderer *renderer,
                                          const Json::Value &obj,
                                          const ComponentFactory &fac);
};

}

#endif
