#include "Vf1_fsm.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

int main(int argc, char **argv, char **env) {
    int i;
    int clk;

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
    vbdHeader("L3T2: LightSeq");

    // Initialize input signals
    top->clk = 1;
    top->rst = 1;
    top->en = 0;  // Initially disabled

    // Run simulation for a defined number of cycles
    for (i = 0; i < 2000; i++) {
        // Toggle clock and dump waveform data
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2 * i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        // Display the output (light sequence)
        vbdBar(top->data_out & 0xFF);  // Send the FSM output to Vbuddy display

        // Update Vbuddy cycle
        vbdCycle(i + 1);

        // Reset logic: assert reset for the first 2 cycles
        top->rst = (i < 2);

        // Enable the FSM on button press
        top->en = vbdFlag();  // The button press will enable the FSM

        // Check for simulation finish condition
        if (Verilated::gotFinish()) exit(0);
    }

    // Clean up and exit
    vbdClose();
    tfp->close();
    delete top; // Clean up allocated memory
    exit(0);
}
