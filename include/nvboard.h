#ifndef __NVBOARD_H__
#define __NVBOARD_H__

#include <component.h>
#include <configs.h>
#include <constrs.h>
#include <keyboard.h>
#include <render.h>
#include <vga.h>

#include <string>

#define BIND_RATE_RT true
#define BIND_RATE_SCR false
#define BIND_DIR_OUT true
#define BIND_DIR_IN false

typedef struct PinMap {
  int len;
  bool is_output;
  union {
    uint16_t pin;
    uint16_t *pins;
  };
  void *signal;
  PinMap *next;
} PinMap;

class NVBoard {
private:
  NVBoardRenderer *renderer;
  std::vector<PinMap *> pin_map_v;
  std::vector<PinMap *> rt_pin_map_v;
  void nvboard_update_input(PinMap *p);
  void nvboard_update_output(PinMap *p);
public:
  /**
   * Init NVBoard.
   *
   * \param vga_clk_cycle
   * \param board
   */
  NVBoard(int vga_clk_cycle = 1, std::string board = "N4");

  /**
   * Quit NVBoard.
   */
  ~NVBoard();

  /**
   * Bind pin of NVBoard.
   *
   * \param signal
   * \param is_rt
   * \param is_output
   * \param len
   * \param ...
   */
  void nvboard_bind_pin(void *signal, bool is_rt, bool is_output, int len, ...);

  /**
   * Update NVBoard.
   * 
   * \return 0 for receiving exit signal
   */
  int nvboard_update();
};

#endif
