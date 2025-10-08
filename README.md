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
git clone https://github.com/JoaoEmanuelCandido/Embarcatech-Tarefa-4-FPGA.git
cd Embarcatech-Tarefa-4-FPGA
```

### 2. **Acione o ambiente do OSS CAD SUITE e Gere o SoC com LiteX**
```sh
source ~/AppsUserWork/LiteX/oss-cad-suite-linux-x64-20250929/oss-cad-suite/environment
cd dotprod/litex
python3 colorlight_i5.py --board i9 --revision 7.2 --build --cpu-type=picorv32  --ecppack-compress
```
É importante destacar que para iniciar o ambiente é necessário te-lo instalado. Em caso de erro com o Yosys, rode o seguinte comando e tente novamente:
```sh
python3 colorlight_i5.py --clean-all
```

### 3. **Compile o firmware**
```sh
cd ../firmware
make
```
Se houver algum erro, tente executar o comando:
```sh
make clean
```
E tente novamente.

### 4. **Grave o bitstream e o firmware na placa**
Primeiro, execute no terminal o seguinte comando:
```sh
which openFPGALoader
```

Copie o caminho descoberto e execute os próximos passos, colocando o caminho no local indicado. O openFPGALoader é uma ferramenta utilizada para carregar arquivos para o FPGA, e já vem por padrão no OSS CAD Suite.

```sh
cd ../litex
/caminho/descoberto -b colorlight-i5 build/colorlight_i5/gateware/colorlight_i5.bit
```

### 5. **Execute e teste via terminal serial**
Execute o seguinte comando, e caso não apareça nada, aperte "enter".
```sh
litex_term /dev/ttyACM0 --kernel ../firmware/main.bin
```

Caso ocorra algum erro com relação a porta, tente mudar para "ttyACM1", ou verifique a porta utilizada no momento em que foi colocado o FPGA no dispositivo.

Após abrir o terminal, digite "reboot". Automaticamente o FPGA será reiniciado, e o programa será executado e mostrado no terminal.

---

## Exemplo de Log de Execução

```
Produto escalar via hardware:  120
Produto escalar via software:  120
Resultado: OK!
```
---

## Testando o Módulo
Para testa o acelerador (módulo systemverilog criado para este projeto), é possível verificar seu funcionamento utilizando o testbench da pasta "tb". Exemplo de código com iverilog (para execução do código específico de demonstração, o usuário deve estar apenas na pasta dotdrop).

```sh
iverilog -g2012 rtl/dotprod.sv tb/dotprod_tb.sv 
./a.out
```

---

## Organização dos Arquivos

- `dotprod/rtl/dotprod.sv` — Bloco SystemVerilog do acelerador
- `dotprod/tb/dotprod_tb.sv` — Testbench do acelerador
- `dotprod/litex/dotprod.py` — Wrapper Python/Migen (CSR)
- `dotprod/litex/colorlight_i5.py` — Arquivo do SoC modificado
- `dotprod/firmware/main.c` — Firmware em C
- `dotprod/firmware/Makefile` — Script de build do firmware
- `dotprod/firmware/linker.ld` — Script do linker

---

## Referências

- [LiteX Documentation](https://github.com/enjoy-digital/litex)
- [LiteX Boards - colorlight_i5](https://github.com/litex-hub/litex-boards/blob/master/litex_boards/targets/colorlight_i5.py)
- [Repositório de Exemplo](https://github.com/dvcirilo/colorlight-i9-examples/blob/main/fibonacci_litex/firmware/main.c)

---

## Autor

João Emanuel Cândido Gonçalves da Silva — 06 de outrubro de 2025
