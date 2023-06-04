#include <SDL2/SDL.h>
#include <component.h>
#include <keyboard.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

void NVBoardViewer::mousedown_handler(const SDL_Event &ev) {
  int x_pos = ev.button.x;
  int y_pos = ev.button.y;
  for (auto i : nrt_components) {
    if ((i->get_interface_type() == INPUT_TYPE) && i->in_rect(x_pos, y_pos)) {
      switch (i->get_component_type()) {
        case BUTTON_TYPE: input_map[i->get_input()] = 1; break;
        case SWICTH_TYPE: input_map[i->get_input()] = i->get_state() ^ 1; break;
      }
    }
  }
}

void NVBoardViewer::mouseup_handler(const SDL_Event &ev) {
  int x_pos = ev.button.x;
  int y_pos = ev.button.y;
  for (auto i : nrt_components) {
    if (i->get_interface_type() == INPUT_TYPE && i->in_rect(x_pos, y_pos)) {
      switch (i->get_component_type()) {
        case BUTTON_TYPE: input_map[i->get_input()] = 0; break;
      }
    }
  }
}

void NVBoardViewer::key_handler(uint8_t scancode, int is_keydown) {
  extern KEYBOARD* kb;
  kb->push_key(scancode, is_keydown);
}

// Return -1 when esc is pressed.
// Else return whether buttons / switches are pressed.
int NVBoardViewer::read_event() {
  SDL_Event ev;
  SDL_PollEvent(&ev);
  switch (ev.type) {
    case SDL_QUIT: return -1;
    case SDL_WINDOWEVENT:
      if(ev.window.event == SDL_WINDOWEVENT_CLOSE) { return -1; }
      break;
    case SDL_MOUSEBUTTONDOWN: mousedown_handler(ev); break;
    case SDL_MOUSEBUTTONUP: mouseup_handler(ev); break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      key_handler(ev.key.keysym.scancode, ev.key.type == SDL_KEYDOWN); break;
  }
  return 0;
}
