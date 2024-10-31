module f1_fsm_with_clktick (
    input  logic clk,                     
    input  logic rst,                     
    input  logic en,                      
    input  logic [15:0] N,                
    output logic [7:0] data_out,         
    output logic tick                     
);

    // Instantiate the clktick module
    clktick #(.WIDTH(16)) clk_divider (
        .clk(clk),      
        .rst(rst),     
        .en(en),       
        .N(N),       
        .tick(tick)    
    );

    // Instantiate the f1_fsm module
    f1_fsm fsm (
        .rst(rst),      
        .en(tick),      
        .clk(clk),      
        .data_out(data_out) 
    );

endmodule
