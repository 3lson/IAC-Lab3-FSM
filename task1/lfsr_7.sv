module lfsr_7(
    input   logic       clk,
    input   logic       rst,
    input   logic       en,
    //Using 4:1 as it is easier to visualise
    output  logic [6:0] data_out
);
    logic [6:0] sreg;

always_ff @ (posedge clk, posedge rst)
    if (rst)
        sreg <= 7'b0000001;
    else
        sreg <= {sreg[5:0], sreg[6] ^ sreg[2]};

assign data_out = sreg;

endmodule
