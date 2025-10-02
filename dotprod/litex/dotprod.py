from migen import Module, Signal, Instance, ClockSignal, ResetSignal
from litex.soc.interconnect.csr import AutoCSR, CSRStorage, CSRStatus

class DotProd(Module, AutoCSR):
    def __init__(self, platform):
        # adiciona o source SystemVerilog
        platform.add_source("../rtl/dotprod.sv")

        # CSRs: entradas e saídas
        self._start  = CSRStorage(1, name="start")
        self._a      = [CSRStorage(32, name=f"a{i}") for i in range(8)]
        self._b      = [CSRStorage(32, name=f"b{i}") for i in range(8)]
        self._done   = CSRStatus(1,  name="done")
        self._result = CSRStatus(64, name="result")

        # sinais internos
        start_sig  = Signal()
        a_sig      = [Signal(32, name=f"a{i}_sig") for i in range(8)]
        b_sig      = [Signal(32, name=f"b{i}_sig") for i in range(8)]
        done_sig   = Signal()
        result_sig = Signal(64, name="result_sig")

        # instância do módulo SystemVerilog
        self.specials += Instance("dotprod",
                                  i_clk=ClockSignal(),
                                  i_rst=ResetSignal(),    # Aqui corresponde a rst_n invertido
                                  i_start=start_sig,
                                  i_a0=a_sig[0], i_a1=a_sig[1], i_a2=a_sig[2], i_a3=a_sig[3],
                                  i_a4=a_sig[4], i_a5=a_sig[5], i_a6=a_sig[6], i_a7=a_sig[7],
                                  i_b0=b_sig[0], i_b1=b_sig[1], i_b2=b_sig[2], i_b3=b_sig[3],
                                  i_b4=b_sig[4], i_b5=b_sig[5], i_b6=b_sig[6], i_b7=b_sig[7],
                                  o_done=done_sig,
                                  o_result=result_sig)

        # mapeamento CSRs <-> sinais
        self.comb += [
            start_sig.eq(self._start.storage),
            *[a_sig[i].eq(self._a[i].storage) for i in range(8)],
            *[b_sig[i].eq(self._b[i].storage) for i in range(8)],
            self._done.status.eq(done_sig),
            self._result.status.eq(result_sig)
        ]
