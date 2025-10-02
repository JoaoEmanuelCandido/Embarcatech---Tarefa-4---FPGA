`timescale 1ps/1ps
module dotprod_tb;

    logic        clk;
    logic        rst;
    logic [31:0] a[0:7];
    logic [31:0] b[0:7];
    logic        start;
    logic        done;
    logic [63:0] result;

    // Instância do módulo
    dotprod dut (
        .clk(clk),
        .rst(rst),            // mudou de rst_n para rst
        .a0(a[0]), .a1(a[1]), .a2(a[2]), .a3(a[3]),
        .a4(a[4]), .a5(a[5]), .a6(a[6]), .a7(a[7]),
        .b0(b[0]), .b1(b[1]), .b2(b[2]), .b3(b[3]),
        .b4(b[4]), .b5(b[5]), .b6(b[6]), .b7(b[7]),
        .start(start),
        .done(done),
        .result(result)
    );

    // Clock
    always #5 clk = ~clk;

    // Arrays de teste globais
    logic [31:0] test_a[0:7];
    logic [31:0] test_b[0:7];

    // Task para rodar o teste
    task automatic run_test;
        integer i;
        begin
            // Copia valores de teste para entradas
            for (i = 0; i < 8; i = i + 1) begin
                a[i] = test_a[i];
                b[i] = test_b[i];
            end

            start = 1;
            @(posedge clk);
            start = 0;

            wait(done);

            $display("==================================================");
            $display("|                Produto Escalar                 |");
            $display("==================================================");
            $write("| A = [");
            for (i = 0; i < 8; i = i + 1) $write(" %0d", a[i]);
            $display(" ]                        |");

            $write("| B = [");
            for (i = 0; i < 8; i = i + 1) $write(" %0d", b[i]);
            $display(" ]                        |");

            $display("| Resultado Final = %0d", result);
            $display("==================================================\n");
        end
    endtask

    // Inicialização
    initial begin
        clk = 0;
        rst = 1;      // reset ativo alto
        start = 0;

        @(posedge clk);
        rst = 0;      // libera reset

        // Inicializa os arrays de teste elemento por elemento
        test_a[0] = 1; test_a[1] = 2; test_a[2] = 3; test_a[3] = 4;
        test_a[4] = 5; test_a[5] = 6; test_a[6] = 7; test_a[7] = 8;

        test_b[0] = 8; test_b[1] = 7; test_b[2] = 6; test_b[3] = 5;
        test_b[4] = 4; test_b[5] = 3; test_b[6] = 2; test_b[7] = 1;

        // Roda o teste
        run_test();

        $finish;
    end

endmodule
