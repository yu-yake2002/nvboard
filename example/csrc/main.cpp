#include <nvboard.h>
#include <Vtop.h>

static TOP_NAME dut;

void nvboard_bind_all_pins(NVBoard *board, Vtop* top);

static void single_cycle() {
  dut.clk = 0; dut.eval();
  dut.clk = 1; dut.eval();
}

static void reset(int n) {
  dut.rst = 1;
  while (n -- > 0) single_cycle();
  dut.rst = 0;
}

int main() {
  NVBoard *board = new NVBoard();
  nvboard_bind_all_pins(board, &dut);

  reset(10);

  while(1) {
    if (board->nvboard_update() != 0) {
      single_cycle();
    } else {
      delete board;
      break;
    }
  }
  return 0;
}
