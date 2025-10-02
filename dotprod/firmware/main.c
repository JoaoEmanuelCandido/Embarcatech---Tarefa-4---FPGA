#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <generated/csr.h>
#include <libbase/console.h>

static void run_dotprod(uint32_t *a, uint32_t *b) {
    // escreve vetores A e B
    dotprod_a0_write(a[0]);
    dotprod_a1_write(a[1]);
    dotprod_a2_write(a[2]);
    dotprod_a3_write(a[3]);
    dotprod_a4_write(a[4]);
    dotprod_a5_write(a[5]);
    dotprod_a6_write(a[6]);
    dotprod_a7_write(a[7]);

    dotprod_b0_write(b[0]);
    dotprod_b1_write(b[1]);
    dotprod_b2_write(b[2]);
    dotprod_b3_write(b[3]);
    dotprod_b4_write(b[4]);
    dotprod_b5_write(b[5]);
    dotprod_b6_write(b[6]);
    dotprod_b7_write(b[7]);

    // start
    dotprod_start_write(1);

    // espera done
    while(dotprod_done_read() == 0);

    // lê resultado
    uint64_t result = dotprod_result_read();
    printf("Resultado: %llu\n", (unsigned long long)result);
}

int main(void) {
    uint32_t a[8] = {1,2,3,4,5,6,7,8};
    uint32_t b[8] = {8,7,6,5,4,3,2,1};

    printf("Teste do DotProd...\n");
    run_dotprod(a, b);

    return 0;
}
