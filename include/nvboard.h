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

#ifndef __NVBOARD_H__
#define __NVBOARD_H__

#include <component.h>
#include <constrs.h>
#include <keyboard.h>
#include <legacy.h>
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
   * \param signal the signal in verilator model
   * \param is_rt whether this signal is a real-time signal
   * \param is_output whether this signal is an output signal
   * \param len the length of signal. If len is more than 1, signal will be a 
   * vector signal
   * \param ... pins in NVBoard, from MSB to LSB
   */
  void NVBoardBindPin(void *signal, bool is_rt, bool is_output, int len, ...);

  /**
   * Bind pin of NVBoard.
   *
   * \param signal the signal in verilator model
   * \param is_rt whether this signal is a real-time signal
   * \param is_output whether this signal is an output signal
   * \param len the length of signal. If len is more than 1, signal will be a 
   * vector signal
   * \param pins pins in NVBoard, from MSB to LSB
   */
  void NVBoardBindPin(void *signal, bool is_rt, bool is_output, int len, const std::list<int> &pins);

  /**
   * Update NVBoard.
   *
   * \return 0 for receiving exit signal
   */
  int NVBoardUpdate();
};

}  // namespace NVBoard

#endif