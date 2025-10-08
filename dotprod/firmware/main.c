#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

#define N_ELEMENTS 8

static int64_t dotprod_software(int32_t *a, int32_t *b, int n);
static void run_dotprod_hardware(int32_t *a, int32_t *b, int64_t *result_hw);
static void test_dotprod(void);

static char *readstr(void)
{
    char c[2];
    static char s[64];
    static int ptr = 0;

    if(readchar_nonblock()) {
        c[0] = readchar();
        c[1] = 0;
        switch(c[0]) {
            case 0x7f:
            case 0x08:
                if(ptr > 0) {
                    ptr--;
                    putsnonl("\x08 \x08");
                }
                break;
            case 0x07:
                break;
            case '\r':
            case '\n':
                s[ptr] = 0x00;
                putsnonl("\n");
                ptr = 0;
                return s;
            default:
                if(ptr >= (sizeof(s) - 1))
                    break;
                putsnonl(c);
                s[ptr] = c[0];
                ptr++;
                break;
        }
    }
    return NULL;
}

static char *get_token(char **str)
{
    char *c, *d;
    c = strchr(*str, ' ');
    if(c == NULL) {
        d = *str;
        *str += strlen(*str);
        return d;
    }
    *c = 0;
    d = *str;
    *str = c + 1;
    return d;
}

static void prompt(void)
{
    printf("RUNTIME>");
}

static void help(void)
{
    puts("Available commands:");
    puts("help         - this command");
    puts("reboot       - reboot CPU");
    puts("led          - led test");
    puts("dotprod      - dot product test");
}

static void reboot(void)
{
    ctrl_reset_write(1);
}

static void toggle_led(void)
{
    int i;
    printf("invertendo led...\n");
    i = leds_out_read();
    leds_out_write(!i);
}

static int64_t dotprod_software(int32_t *a, int32_t *b, int n)
{
    int64_t acc = 0;
    for (int i = 0; i < n; i++)
        acc += (int64_t)a[i] * b[i];
    return acc;
}

static void run_dotprod_hardware(int32_t *a, int32_t *b, int64_t *result_hw)
{
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

    dotprod_start_write(1);
    while(!dotprod_done_read());
    dotprod_start_write(0);

    uint64_t result_raw = dotprod_result_read();
    *result_hw = (int64_t)result_raw;
}

static void test_dotprod(void)
{
    int32_t a[N_ELEMENTS] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t b[N_ELEMENTS] = {8, 7, 6, 5, 4, 3, 2, 1};

    int64_t result_sw = dotprod_software(a, b, N_ELEMENTS);
    int64_t result_hw;

    run_dotprod_hardware(a, b, &result_hw);

    printf("\n--- Teste do Acelerador Dot Product ---\n");
    printf("[SW] Resultado esperado: %ld\n", (long)result_sw);
    printf("[HW] Resultado lido:     %ld\n", (long)result_hw);

    if (result_sw == result_hw)
        printf("SUCESSO: resultados iguais.\n");
    else
        printf("FALHA: resultados diferentes!\n");
}

static void console_service(void)
{
    char *str, *token;
    str = readstr();
    if(str == NULL) return;

    token = get_token(&str);
    if(strcmp(token, "help") == 0)
        help();
    else if(strcmp(token, "reboot") == 0)
        reboot();
    else if(strcmp(token, "led") == 0)
        toggle_led();
    else if(strcmp(token, "dotprod") == 0)
        test_dotprod();
    else
        puts("Comando desconhecido. Digite 'help'.");

    prompt();
}

int main(void)
{
#ifdef CONFIG_CPU_HAS_INTERRUPT
    irq_setmask(0);
    irq_setie(1);
#endif
    uart_init();

    printf("Hellorld!\n");
    help();
    prompt();

    while(1)
        console_service();

    return 0;
}
