/* verilator lint_off WIDTH */

module video
(
    input   wire        clock,
    output  reg [3:0]   r,
    output  reg [3:0]   g,
    output  reg [3:0]   b,
    output  wire        hs,
    output  wire        vs
);

// ---------------------------------------------------------------------
// Тайминги для горизонтальной|вертикальной развертки (640x400)
// ---------------------------------------------------------------------
parameter
    hz_vs = 640, vt_vs = 400,
    hz_ft =  16, vt_ft =  12,
    hz_sy =  96, vt_sy =   2,
    hz_bk =  48, vt_bk =  35,
    hz_al = 800, vt_al = 449;

assign hs = x  < (hz_bk + hz_vs + hz_ft); // NEG.
assign vs = y >= (vt_bk + vt_vs + vt_ft); // POS.
// ---------------------------------------------------------------------
wire        xmax = (x == hz_al - 1);
wire        ymax = (y == vt_al - 1);
reg  [10:0] x    = 0;
reg  [10:0] y    = 0;
// ---------------------------------------------------------------------

// Вывод видеосигнала
always @(posedge clock) begin

    // Кадровая развертка
    x <= xmax ?         0 : x + 1;
    y <= xmax ? (ymax ? 0 : y + 1) : y;
    {r, g, b} <= 12'h000;

    // Вывод окна видеоадаптера
    if (x >= hz_bk && x < hz_vs+hz_bk && y >= vt_bk && y < vt_vs+vt_bk)
    begin
         {r, g, b} <= 12'h008;
    end

end

endmodule
