/*
 * Модуль, который генерирует HOLD=0 для процессора,
 * задерживая для достижения определенной частоты
 */

module clockdiv
(
    input               reset_n,
    input               clock,      // Опорная частота
    input               active,     // =1 Включить
    input        [7:0]  freq,       // Запрошенная частота 12.5 = 125
    input        [7:0]  fref,       // Опорная частота 25 = 250
    output reg          hold        // =1 Работает =0 Удерживается
);

initial hold = 1'b1;

reg [7:0] latch = 0;

always @(negedge clock)
if      (reset_n == 0) begin hold <= 1'b1; end // Чтобы процессор работал
else if (active  == 0) begin hold <= 1'b0; end
else begin

    hold  <= latch < freq;
    latch <= latch == (fref-1) ? 0 : latch + 1;

end

endmodule
