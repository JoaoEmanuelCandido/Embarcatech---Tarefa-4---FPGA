#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Inclui o cabeçalho gerado pelo LiteX
// ESTE ARQUIVO DEVE CONTER AS FUNÇÕES dotprod_aX_write e dotprod_bX_write
#include <generated/csr.h>
#include <libbase/console.h>
#include <libbase/uart.h>

#define N_ELEMENTS 8

// Função para calcular o produto escalar em software
// Usamos int64_t para garantir que o acumulador não sature,
// pois a multiplicação de dois int32_t pode resultar em até 64 bits.
int64_t dotprod_software(int32_t *a, int32_t *b, int n) {
    int64_t sum = 0;
    for (int i = 0; i < n; i++) {
        // Multiplicação de 32-bit signed que é promovida para 64-bit signed
        sum += (int64_t)a[i] * (int64_t)b[i];
    }
    return sum;
}

// Função para interagir com o acelerador de hardware
static uint64_t run_dotprod_hardware(int32_t *a, int32_t *b) {
    printf(">> Escrevendo dados nos CSRs...\n");
    
    // Escreve vetor A (a0 a a7)
    dotprod_a0_write(a[0]);
    dotprod_a1_write(a[1]);
    dotprod_a2_write(a[2]);
    dotprod_a3_write(a[3]);
    dotprod_a4_write(a[4]);
    dotprod_a5_write(a[5]);
    dotprod_a6_write(a[6]);
    dotprod_a7_write(a[7]);

    // Escreve vetor B (b0 a b7)
    dotprod_b0_write(b[0]);
    dotprod_b1_write(b[1]);
    dotprod_b2_write(b[2]);
    dotprod_b3_write(b[3]);
    dotprod_b4_write(b[4]);
    dotprod_b5_write(b[5]);
    dotprod_b6_write(b[6]);
    dotprod_b7_write(b[7]);

    printf(">> Enviando sinal de START...\n");
    // Starta a operação
    dotprod_start_write(1);
    
    // Espera pelo sinal DONE (Polling)
    while(!dotprod_done_read()) {
        // Aguarda a conclusão
    }
    
    // Desliga o START (se necessário, para reuso)
    dotprod_start_write(0);

    // Lê o resultado (64 bits)
    return dotprod_result_read();
}

int main(void) {
    // Atraso inicial para estabilização
    for (volatile int i = 0; i < 100000; i++);
    
    printf("--- Teste do Acelerador Dot Product ---\n");
    
    // Dados de teste (Exemplo: 1*(-1) + 2*2 + ... = -1 + 4 + 9 + 16 + 25 + 36 + 49 + 64 = 194)
    int32_t a[N_ELEMENTS] = {-1, 2, 3, 4, 5, 6, 7, 8};
    int32_t b[N_ELEMENTS] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    int64_t result_sw;
    uint64_t result_hw_raw;
    int64_t result_hw_signed;

    // 1. Cálculo em Software (SW)
    result_sw = dotprod_software(a, b, N_ELEMENTS);
    printf("\n[SW] Resultado esperado: %lld\n", (long long)result_sw);

    // 2. Cálculo no Hardware (HW)
    result_hw_raw = run_dotprod_hardware(a, b);
    
    // Faz o cast do uint64_t lido para int64_t para impressão correta de signed number
    result_hw_signed = (int64_t)result_hw_raw;
    printf("[HW] Resultado lido: %lld\n", (long long)result_hw_signed);

    // 3. Verificação
    printf("\n--- Verificação ---\n");
    if (result_sw == result_hw_signed) {
        printf("SUCESSO: Os resultados em Software e Hardware são iguais.\n");
    } else {
        printf("FALHA: Resultados diferentes! SW: %lld, HW: %lld\n", (long long)result_sw, (long long)result_hw_signed);
    }
    
    // Loop infinito para manter a UART ativa
    while(1) {
        // Não faz nada, apenas espera
    }

    return 0;
}
