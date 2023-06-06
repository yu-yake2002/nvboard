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

// macro concatenation
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)

// vga macros
#define VGA_POS_EDGE(constr) (concat(vga_, constr) == 1 && concat(vga_pre_, constr) == 0)
#define VGA_NEG_EDGE(constr) (concat(vga_, constr) == 0 && concat(vga_pre_, constr) == 1)
