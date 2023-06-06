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

#ifndef _VFPGA_LEGACY_H
#define _VFPGA_LEGACY_H

/**
 * Init NVBoard.
 * 
 * \deprecated Please use the interface of NVBoardController instead
*/
void nvboard_init();

/**
 * Quit NVBoard.
 * 
 * \deprecated Please use the interface of NVBoardController instead
*/
void nvboard_quit();

/**
 * Bind NVBoard's pin and signal together.
 * 
 * \deprecated Please use the interface of NVBoardController instead
 * \param signal the signal in verilator model
 * \param is_rt whether this signal is a real-time signal
 * \param is_output whether this signal is an output signal
 * \param len the length of signal. If len is more than 1, signal will be a 
 * vector signal
 * \param ... pins in NVBoard, from MSB to LSB
*/
void nvboard_bind_pin(void *signal, bool is_rt, bool is_output, int len, ...);

/**
 * Update NVBoard.
 * 
 * \deprecated Please use the interface of NVBoardController instead
*/
void nvboard_update();

#endif