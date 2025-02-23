#!/bin/sh

# cleanup
rm -rf obj_dir
rm -f *.vcd

# run Verilator to translate Verilog into C++, including C++ testbench
verilator -Wall --cc --trace F1_Lights.sv delay.sv f1_fsm.sv lfsr.sv clktick.sv --exe F1_Lights_tb.cpp

# build C++ project via make automatically generated by Verilator
make -j -C obj_dir/ -f VF1_Lights.mk VF1_Lights

# run executable simulation file
echo "\nRunning simulation"
obj_dir/VF1_Lights
echo "\nSimulation completed"