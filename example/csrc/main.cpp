#include <nvboard.h>
#include <Vtop.h>

void nvboard_bind_all_pins(NVBoard::NVBoardController *board, Vtop* top);

static inline void single_cycle(TOP_NAME &dut) {
  dut.clk = 0; dut.eval();
  dut.clk = 1; dut.eval();
}

static inline void reset(TOP_NAME &dut, int n) {
  dut.rst = 1;
  while (n -- > 0) single_cycle(dut);
  dut.rst = 0;
}

int main() {
  Vtop model = Vtop();
  NVBoard::NVBoardController *controller = new NVBoard::NVBoardController();
  nvboard_bind_all_pins(controller, &model);

  reset(model, 10);
  while(1) {
    if (controller->NVBoardUpdate() != 0) {
      single_cycle(model);
    } else {
      break;
    }
  }
  
  delete controller;
  return 0;
}