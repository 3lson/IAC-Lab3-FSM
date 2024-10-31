module f1_fsm (
    input   logic       rst,
    input   logic       en,
    input   logic       clk,
    output  logic [7:0] data_out
);
    // Define states
    typedef enum {S0, S1, S2, S3, S4, S5, S6, S7, S8} my_state;
    my_state current_state, next_state;

    // Next state logic
    always_comb  
        case (current_state)
            S0: next_state = (en==1'b1) ? S1 : current_state;
            S1: next_state = (en==1'b1) ? S2 : current_state;
            S2: next_state = (en==1'b1) ? S3 : current_state;
            S3: next_state = (en==1'b1) ? S4 : current_state;
            S4: next_state = (en==1'b1) ? S5 : current_state;
            S5: next_state = (en==1'b1) ? S6 : current_state;
            S6: next_state = (en==1'b1) ? S7 : current_state;
            S7: next_state = (en==1'b1) ? S8 : current_state;
            S8: next_state = (en==1'b1) ? S0 : current_state;
            default: next_state = S0;
        endcase

    // State transition with synchronous reset
    always_ff @(posedge clk) begin
        if (rst) begin
            current_state <= S0; // Reset state
        end else begin
            current_state <= next_state; // State transition
        end
    end

    // Output logic
    always_comb
        case (current_state)
            S0: data_out = 8'b0;
            S1: data_out = 8'b1;
            S2: data_out = 8'b11;
            S3: data_out = 8'b111;
            S4: data_out = 8'b1111;
            S5: data_out = 8'b11111;
            S6: data_out = 8'b111111;
            S7: data_out = 8'b1111111;
            S8: data_out = 8'b11111111;
            default: data_out = 8'b0;
        endcase
endmodule
