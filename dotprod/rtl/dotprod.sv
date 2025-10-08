`timescale 1ns/1ps
module dotprod #(
    parameter N = 8,
    parameter IN_WIDTH = 32,
    parameter OUT_WIDTH = 64
)(
    input  logic                  clk,
    input  logic                  rst,
    input  logic                  start,
    input  logic [IN_WIDTH-1:0]   a0, a1, a2, a3, a4, a5, a6, a7,
    input  logic [IN_WIDTH-1:0]   b0, b1, b2, b3, b4, b5, b6, b7,
    output logic                  done,
    output logic [OUT_WIDTH-1:0]  result
);

    logic [IN_WIDTH-1:0] a_array [0:N-1];
    logic [IN_WIDTH-1:0] b_array [0:N-1];

    typedef enum logic [1:0] {IDLE=0, LOAD=1, CALC=2, DONE=3} state_t;
    state_t state, state_n;

    logic [$clog2(N)-1:0] idx, idx_n;
    logic [OUT_WIDTH-1:0] acc, acc_n;

    always_ff @(posedge clk or posedge rst) begin
        if (rst) begin
            state <= IDLE;
            idx   <= 0;
            acc   <= 0;
        end else begin
            state <= state_n;
            idx   <= idx_n;
            acc   <= acc_n;
        end
    end

    // Copia os vetores no estado LOAD
    always_ff @(posedge clk) begin
        if (state == LOAD) begin
            a_array[0] <= a0; a_array[1] <= a1; a_array[2] <= a2; a_array[3] <= a3;
            a_array[4] <= a4; a_array[5] <= a5; a_array[6] <= a6; a_array[7] <= a7;
            b_array[0] <= b0; b_array[1] <= b1; b_array[2] <= b2; b_array[3] <= b3;
            b_array[4] <= b4; b_array[5] <= b5; b_array[6] <= b6; b_array[7] <= b7;
        end
    end

    always_comb begin
        state_n = state;
        idx_n   = idx;
        acc_n   = acc;
        done    = 0;

        case(state)
            IDLE: begin
                if (start)
                    state_n = LOAD;
            end

            LOAD: begin
                idx_n = 0;
                acc_n = 0;
                state_n = CALC;
            end

            CALC: begin
                acc_n = acc + $signed(a_array[idx]) * $signed(b_array[idx]);
                if (idx == N-1)
                    state_n = DONE;
                else
                    idx_n = idx + 1;
            end

            DONE: begin
                done = 1;
                if (!start) state_n = IDLE;
            end
        endcase
    end

    assign result = acc;

endmodule
