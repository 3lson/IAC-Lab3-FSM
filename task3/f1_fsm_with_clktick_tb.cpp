#include "Vf1_fsm.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

#define MAX_SIM_CYC 1000 // Maximum simulation cycles

int main(int argc, char **argv) {
    // Verilator setup for command-line arguments
    Verilated::commandArgs(argc, argv);

    // Instantiate the FSM module
    Vf1_fsm* top = new Vf1_fsm;

    // Setup VCD tracing
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("f1_fsm.vcd");  // Change to your FSM file name

    // Initialize Vbuddy interface
    if (vbdOpen() != 1) return -1;
    vbdHeader("L3T3: ClkTick");
    vbdSetMode(1); // Flag mode set to one-shot

    // Initialize inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 1; // Enable signal for the FSM
    top->N = 24; // Set N for 1-second tick

    // Run simulation for a defined number of cycles
    for (int simcyc = 0; simcyc < MAX_SIM_CYC; simcyc++) {
        // Toggle clock and dump variables into VCD file
        for (int tick = 0; tick < 2; tick++) {
            tfp->dump(2 * simcyc + tick);
            top->clk = !top->clk;
            top->eval();
        }

        // Display current state output to Vbuddy
        vbdBar(top->data_out & 0xFF);

        // Reset logic
        top->rst = (simcyc < 2); // Reset for the first 2 cycles

        // Cycle counter
        vbdCycle(simcyc);

        // Check for simulation finish condition
        if (Verilated::gotFinish() || vbdGetkey() == 'q') {
            exit(0);
        }
    }

    // Cleanup
    vbdClose();
    tfp->close();
    exit(0);
}
