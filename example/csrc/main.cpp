#include <nvboard.h>
#include <Vtop.h>

void nvboard_bind_all_pins(NVBoardController *board, Vtop* top);

static inline void single_cycle(TOP_NAME *dut) {
  dut->clk = 0; dut->eval();
  dut->clk = 1; dut->eval();
}

static inline void reset(Vtop *dut, int n) {
  dut->rst = 1;
  while (n -- > 0) single_cycle(dut);
  dut->rst = 0;
}

int main() {
  Vtop *model = new Vtop();
  NVBoardView *view = new NVBoardView();
  NVBoardController *controller = new NVBoardController(view);
  nvboard_bind_all_pins(controller, model);

  reset(model, 10);
  while(1) {
    if (controller->Update() != 0) {
      single_cycle(model);
    } else {
      break;
    }
  }
  
  delete controller;
  delete view;
  delete model;
  return 0;
}
