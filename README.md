# Tarefa 04 - SoC com LiteX: Acelerador de Produto Escalar via CSR

## Descrição do Projeto

Este projeto implementa um acelerador de produto escalar para o SoC LiteX, utilizando o target `colorlight_i5` (i9v7). O acelerador é desenvolvido em SystemVerilog e integrado ao SoC via barramento CSR, permitindo comunicação eficiente entre firmware e hardware. O firmware em C interage com o acelerador via UART, enviando dados, acionando o cálculo e exibindo o resultado junto ao cálculo em software para comparação.

---

## Arquitetura do SoC

- **CPU:** RISC-V (LiteX)
- **Periféricos:** UART, SDRAM, SPI Flash, LED, acelerador de produto escalar
- **Acelerador:** Produto escalar entre dois vetores de 8 elementos (32 bits cada)
- **Comunicação:** Barramento CSR (Control & Status Register)

### Mapa de CSRs do acelerador

| Nome         | Endereço Offset | Descrição                | Tipo      |
|--------------|----------------|--------------------------|-----------|
| a0..a7       | 0x04..0x20     | Elementos do vetor A     | Escrita   |
| b0..b7       | 0x24..0x40     | Elementos do vetor B     | Escrita   |
| start        | 0x00           | Inicia o cálculo         | Escrita   |
| done         | 0x44           | Indica término do cálculo| Leitura   |
| result       | 0x48/0x4C      | Resultado (64 bits)      | Leitura   |

---

## Instruções de Compilação e Execução

### 1. **Clone o repositório**
```sh
git clone https://github.com/JoaoEmanuelCandido/Embarcatech---Tarefa-4---FPGA.git
cd Embarcatech---Tarefa-4---FPGA
```

### 2. **Gere o SoC com LiteX**
```sh
python3 dotprod/litex/colorlight_i5.py --build
```

### 3. **Compile o firmware**
```sh
cd dotprod/firmware
make
```

### 4. **Grave o bitstream e o firmware na placa**
```sh
python ../litex/colorlight_i5.py --load
```
Ou, para carregar só o firmware via terminal LiteX:
```sh
litex_term /dev/ttyUSBx --kernel main.bin
```

### 5. **Execute e teste via terminal serial**
- Conecte ao terminal serial (ex: `minicom`, `screen`, `litex_term`)
- O firmware irá mostrar os resultados do produto escalar em hardware e software.

---

## Exemplo de Log de Execução

```
Produto escalar via hardware:  12345678
Produto escalar via software:  12345678
Resultado: OK!
```
---

## Arquivos do Projeto

- `dotprod/rtl/dotprod.sv` — Bloco SystemVerilog do acelerador
- `dotprod/rtl/dotprod_tb.sv` — Testbench do acelerador
- `dotprod/litex/dotprod.py` — Wrapper Python/Migen (CSR)
- `dotprod/litex/colorlight_i5.py` — Arquivo do SoC modificado
- `dotprod/firmware/main.c` — Firmware em C
- `dotprod/firmware/Makefile` — Script de build do firmware
- `dotprod/firmware/linker.ld` — Script do linker
- `dotprod/litex/build/colorlight_i5/software/include/generated/csr.h` — Header gerado dos CSRs

---

## Referências

- [LiteX Documentation](https://github.com/enjoy-digital/litex)
- [LiteX Boards - colorlight_i5](https://github.com/litex-hub/litex-boards/blob/master/litex_boards/targets/colorlight_i5.py)

---

## Autor

João Emanuel Cândido Gonçalves da Silva — 06 de outrubro de 2025
