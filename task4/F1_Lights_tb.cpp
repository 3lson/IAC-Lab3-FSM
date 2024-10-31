#include "verilated.h"
#include "verilated_vcd_c.h"
#include "VF1_Lights.h"

#include "vbuddy.cpp"     // include vbuddy code
#define MAX_SIM_CYC 100000

int main(int argc, char **argv, char **env) {
  int simcyc;     // simulation clock count
  int tick;       // each clk cycle has two ticks for two edges
  int lights = 0; // state to toggle LED lights
  bool isCounting = false;
  bool isWaiting = false;

  Verilated::commandArgs(argc, argv);
  // init top verilog instance
  VF1_Lights * top = new VF1_Lights;
  // init trace dump
  Verilated::traceEverOn(true);
  VerilatedVcdC* tfp = new VerilatedVcdC;
  top->trace (tfp, 99);
  tfp->open ("F1_Lights.vcd");
 
  // init Vbuddy
  if (vbdOpen()!=1) return(-1);
  vbdHeader("L3T4:F1 Lights");
  vbdSetMode(1);        // Flag mode set to one-shot

  // initialize simulation inputs
  top->clk = 1;
  top->rst = 0;
  top->trigger = 0;
  top->n = vbdValue();
  
  // run simulation for MAX_SIM_CYC clock cycles
  for (simcyc=0; simcyc<MAX_SIM_CYC; simcyc++) {
    // dump variables into VCD file and toggle clock
    for (tick=0; tick<2; tick++) {
      tfp->dump (2*simcyc+tick);
      top->clk = !top->clk;
      top->eval ();
    }

    // Display toggle neopixel
    /*if (top->time_out) {
      vbdBar(lights);
      lights = lights ^ 0xFF;
    }*/
    vbdBar(top->data_out);
    // set up input signals of testbench
    top->rst = (simcyc < 2);    // assert reset for 1st cycle
    top->trigger = vbdFlag();
    top->n = vbdValue();
    vbdCycle(simcyc);


    if (top->data_out==0xFF) {
      isCounting = false;
    }

    if (!isCounting) {
      if (top->data_out == 0x00 && !isWaiting) {
        vbdInitWatch();
        isWaiting = true;
      }
    }

    if(isWaiting) {
      if (vbdFlag()) {
        top->count = vbdElapsed();
        int time = top->bcd;
        vbdHex(1, time & 0xF); //7-segment display
        vbdHex(2, (time >> 4) & 0xF);
        vbdHex(3, (time >> 8) & 0xF);
        vbdHex(4, (time >> 16) & 0xF);
        isWaiting = false;
        isCounting = true;
      }
    }

    if (Verilated::gotFinish())  exit(0);
  }

  vbdClose();     // ++++
  tfp->close(); 
  exit(0);
}