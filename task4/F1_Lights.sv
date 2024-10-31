module F1_Lights(
    input   logic               clk,        // clock signal
    input   logic               rst,        // reset signal
    input   logic  [7:0]       count,
    input   logic               en,         // enable signal
    input   logic               trigger,    // trigger input signal
    input   logic [4:0]         n,          // no of clock cycle delay
    output  logic [7:0]         data_out,
    output  logic [11:0]        bcd
); 


logic tick;
logic [6:0] K; // no of clock cycle delay
logic time_out;
logic cmd_seq;
logic cmd_delay; 
logic f1_fsm_en = 0;
 

lfsr LFSR(
    .clk (clk),
    .rst (rst),
    .en (1'b1),
    .data_out (K)
);

clktick ClkTick(
    .clk (clk),
    .rst (rst),
    .en (cmd_seq),
    .N (n),
    .tick (tick)
);

delay Delay(
    .clk (clk),
    .rst (rst),
    .trigger (cmd_delay),
    .n (K),
    .time_out (time_out)
);

bin2bcd myDecoder (
  .x (count),
  .BCD (bcd)
);

assign f1_fsm_en = cmd_seq ? tick : time_out;

f1_fsm F1FSM(
    .clk (clk),
    .rst (rst),
    .en (f1_fsm_en),
    .trigger (trigger),
    .data_out (data_out),
    .cmd_seq (cmd_seq),
    .cmd_delay (cmd_delay)
);

endmodule