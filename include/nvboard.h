#ifndef __NVBOARD_H__
#define __NVBOARD_H__

#include <component.h>
#include <constrs.h>
#include <keyboard.h>
#include <vga.h>
#include <view.h>

#include <list>
#include <string>

#define BIND_RATE_RT true
#define BIND_RATE_SCR false
#define BIND_DIR_OUT true
#define BIND_DIR_IN false

namespace NVBoard {

struct PinMap {
  int len_;
  bool is_output_;
  union {
    uint16_t pin_;
    uint16_t *pins_;
  };
  void *signal_;
  PinMap(void *signal, bool is_output, int len, uint16_t pin);
  PinMap(void *signal, bool is_output, int len, uint16_t *pins);
  ~PinMap();
};

class NVBoardController {
 private:
  NVBoardView *viewer_;
  std::list<PinMap *> pin_map_v_;
  std::list<PinMap *> rt_pin_map_v_;
  void UpdateInputPin(PinMap *p);
  void UpdateOutputPin(PinMap *p);

 public:
  /**
   * Init NVBoard.
   *
   * \param board the configuration directory of board
   */
  NVBoardController(std::string board = "N4");

  /**
   * Quit NVBoard.
   */
  ~NVBoardController();

  /**
   * Bind pin of NVBoard.
   *
   * \param signal
   * \param is_rt
   * \param is_output
   * \param len
   * \param ...
   */
  void NVBoardBindPin(void *signal, bool is_rt, bool is_output, int len, ...);

  /**
   * Update NVBoard.
   *
   * \return 0 for receiving exit signal
   */
  int NVBoardUpdate();
};

}  // namespace NVBoard

#endif