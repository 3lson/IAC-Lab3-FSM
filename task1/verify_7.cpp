#include "gtest/gtest.h"
#include "Vdut.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

Vdut* top;
VerilatedVcdC* tfp;
unsigned int ticks = 0;

class TestDut : public ::testing::Test
{
public:
    void SetUp() override
    {
        initializeInputs();  // Initialize inputs
        runReset();          // Reset the DUT
    }

    void TearDown() override
    {
        // No specific tear-down actions needed, handled in main.
    }

    void initializeInputs()
    {
        top->clk = 0; // Set clock low
        top->rst = 0; // Deassert reset
        top->en = 1;  // Enable the DUT if needed
    }

    // Apply a reset signal
    void runReset()
    {
        top->rst = 1;  // Assert reset
        runSimulation();
        top->rst = 0;  // Deassert reset
    }

    // Simulate for two clock cycles, evaluate the DUT, and dump waveform data
    void runSimulation()
    {
        for (int clk = 0; clk < 2; clk++)
        {
            top->eval();                // Evaluate the DUT
            tfp->dump(2 * ticks + clk); // Dump the waveform data
            top->clk = !top->clk;       // Toggle the clock
        }
        ticks++;

        if (Verilated::gotFinish())
        {
            exit(0);
        }
    }

protected:
};

TEST_F(TestDut, InitialStateTest)
{
    top->rst = 1;       // Assert reset
    runSimulation();    // Run the simulation after asserting reset
    EXPECT_EQ(top->data_out, 0b0001); // Check expected output
}

// Generate the next state for a 7-bit LFSR
int generateLFSR7(int state)
{
    int b0 = (state >> 0) & 1;
    int b1 = (state >> 1) & 1;
    int b2 = (state >> 2) & 1;
    int b3 = (state >> 3) & 1;
    int b4 = (state >> 4) & 1;
    int b5 = (state >> 5) & 1;
    int b6 = (state >> 6) & 1;

    return (b5 << 6 | b4 << 5 | b3 << 4 | b2 << 3 | b1 << 2 | b0 << 1 |
            (b2 ^ b6));
}

TEST_F(TestDut, SequenceTest)
{
    top->rst = 1;       // Assert reset
    runSimulation();    // Run simulation after reset
    top->rst = 0;       // Deassert reset

    int exp = 0b000'0001; // Initial expected value

    // Run sequence test with 2^7 cycles
    for (int i = 0; i < (1 << 7); i++)
    {
        exp = generateLFSR7(exp); // Generate the next expected value
        runSimulation();          // Run simulation for the next clock cycles
        EXPECT_EQ(top->data_out, exp); // Check against expected output
    }
}

int main(int argc, char **argv)
{
    top = new Vdut;                  // Initialize DUT
    tfp = new VerilatedVcdC;         // Initialize VCD tracing

    Verilated::traceEverOn(true);    // Enable tracing
    top->trace(tfp, 99);             // Attach trace
    tfp->open("waveform.vcd");       // Open trace file

    testing::InitGoogleTest(&argc, argv); // Initialize Google Test
    int res = RUN_ALL_TESTS();            // Run all tests

    top->final(); // Finalize DUT
    tfp->close(); // Close trace file

    delete top;   // Clean up allocated DUT
    delete tfp;   // Clean up allocated trace

    return res;   // Return test result
}
