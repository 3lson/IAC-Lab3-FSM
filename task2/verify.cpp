#include "gtest/gtest.h"
#include "Vdut.h"  // Verilator model for the DUT
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <vector>

// Declare raw pointers for DUT and trace file, to be initialized in main
Vdut* top = nullptr;
VerilatedVcdC* tfp = nullptr;
unsigned int ticks = 0;  // Clock ticks counter for tracing

class TestDut : public ::testing::Test
{
public:
    void SetUp() override
    {
        initializeInputs(); // Initialize DUT inputs
        runReset();         // Apply reset
    }

    void TearDown() override
    {
        // No cleanup needed as `top` and `tfp` will be managed in main
    }

    void initializeInputs()
    {
        top->clk = 0;
        top->rst = 0;
        top->en = 1;
    }

    void runReset()
    {
        top->rst = 1;   // Apply reset
        runSimulation(); // Run simulation for reset to propagate
        top->rst = 0;   // Deassert reset
    }

    // Runs the simulation for one clock cycle, evaluates the DUT, and dumps waveform.
    void runSimulation()
    {
        for (int clk = 0; clk < 2; clk++)
        {
            top->eval();                // Evaluate DUT
            tfp->dump(2 * ticks + clk); // Dump waveform data
            top->clk = !top->clk;       // Toggle clock signal
        }
        ticks++;

        if (Verilated::gotFinish())     // Check for simulation finish
        {
            exit(0);
        }
    }
};

// Test the initial state after reset
TEST_F(TestDut, InitialStateTest)
{
    EXPECT_EQ(top->data_out, 0x00);  // Check the initial state
}

// Test FSM sequence
TEST_F(TestDut, FSMTest)
{
    // Expected FSM sequence
    std::vector<int> expected = {
        0b0000'0000,
        0b0000'0001,
        0b0000'0011,
        0b0000'0111,
        0b0000'1111,
        0b0001'1111,
        0b0011'1111,
        0b0111'1111,
        0b1111'1111,
        0b0000'0000
    };

    for (int exp : expected)
    {
        EXPECT_EQ(top->data_out, exp); // Verify each state in the sequence
        runSimulation();               // Advance the simulation to the next state
    }
}

int main(int argc, char **argv)
{
    // Initialize Verilator's trace and DUT
    top = new Vdut;
    tfp = new VerilatedVcdC;

    Verilated::traceEverOn(true);   // Enable tracing
    top->trace(tfp, 99);            // Set up tracing for the DUT
    tfp->open("waveform.vcd");      // Open the VCD file for waveform output

    testing::InitGoogleTest(&argc, argv); // Initialize Google Test
    int result = RUN_ALL_TESTS();         // Run all tests

    // Cleanup DUT and trace file after tests are complete
    top->final();
    tfp->close();

    delete top;
    delete tfp;

    return result;
}
