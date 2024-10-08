/* verilator lint_off WIDTH */

module mmap
(
    input               reset_n,    // =0 Сигнал сброса
    input               clock,      // Частота процессора
    input               m0,
    input               hold,
    input        [15:0] address,
    output  reg  [ 7:0] i_data,     // Входящие данные в процессор
    input        [ 7:0] o_data,     // Исходящие из процессора данных
    input               we,
    output  reg  [ 7:0] portin,
    input               portrd,
    input               portwe,

    // Управление памятью
    output reg   [14:0] rom_address, // 32k
    output reg   [16:0] ram_address, // 128k
    input        [ 7:0] rom_idata,
    input        [ 7:0] ram_idata,
    output reg          ram_we,

    // Видеопамять
    output              vidpage,
    output reg   [ 2:0] border,

    // Клавиатура, микрофон
    input        [ 7:0] kbd,
    input               mic,
    output reg          spkr,

    // Регистры AY
    output reg  [3:0]   ay_reg,
    output reg  [7:0]   ay_data_o,
    input       [7:0]   ay_data_i,
    output reg          ay_req,

    // Интерфейс
    output reg          sd_signal,  // 0->1 Команда на позитивном фронте
    output reg  [ 1:0]  sd_cmd,     // ID команды
    input       [ 7:0]  sd_din,     // Данные от SD
    output reg  [ 7:0]  sd_out,     // Запись на SD
    input               sd_busy,    // =1 Устройство занято
    input               sd_timeout  // =1 Истек таймаут
);

initial begin

    sd_signal = 0;
    sd_cmd    = 0;
    sd_out    = 0;

end

wire [15:0] A = address;

// ---------------------------------------------------------------------

// http://speccy.info/Порт_7FFD
reg  [7:0]  port7ffd = 0;

// Выбранный банк памяти
wire [2:0]  bank     = port7ffd[5] ? 3'd0 : port7ffd[2:0];

// Видеоадаптер (5-й или 7-й банк). Если D5, заблокирован на 5-й экран
assign      vidpage  = port7ffd[5] ? 1'b0 : port7ffd[3];

// Если D5, то зафиксировать 48k ROM
wire        rompage  = port7ffd[5] ? 1'b1 : port7ffd[4];

// ---------------------------------------------------------------------

// Роутеры
always @(*) begin

    i_data      = ram_idata;
    portin      = 8'hFF;
    ram_we      = 1'b0;
    ram_address = A[15:0];
    rom_address = {rompage, A[13:0]};

    // 16-битная адресная шина
    case (A[15:14])

        // Выбрана область ROM
        2'b00: begin i_data = rom_idata; end

        // Всегда отображается банк 5/2
        2'b01,
        2'b10: begin ram_we = we; ram_address = {A[14], 2'b01, A[13:0]}; end

        // Выбор банка памяти
        2'b11: begin

            ram_we = we & ~port7ffd[7]; // Активен когда 0
            ram_address = {bank, A[13:0]};

        end

    endcase

    // AY-чип
    if      (A == 16'hFFFD)   portin = ay_reg;
    else if (A == 16'hBFFD)   portin = ay_data_i;
    // SD-карта
    else if (A[7:0] == 8'h0F) portin = sd_din;
    else if (A[7:0] == 8'h1F) portin = {sd_timeout, 6'b000000, sd_busy};
    // ZX Spectrum +2/+3
    else if (A == 16'h1FFD)   portin = 8'hFF;
    // Порт 7FFD
    else if (A[7:0] == 8'hFD) portin = port7ffd;
    // Клавиатура и микрофон (базовый запрос)
    else if (A[0]   == 1'b0)  portin = {1'b1, /*D6*/ mic, 1'b1, /*D4..D0*/ kbd[4:0]};
    // Kempston-джойстик
    else if (A[7:5] == 3'b0)  portin = 0;

end

// Запись в порты
always @(posedge clock) begin

    sd_signal <= 1'b0;

    // Выбор 128K
    if (reset_n == 0) begin

        port7ffd  <= 0;
        ay_req    <= 0;

    end
    else if (portwe && hold) begin

        // AY адрес регистра
        if      (A == 16'hFFFD) begin ay_reg <= o_data[3:0]; end
        // AY данные
        else if (A == 16'hBFFD) begin ay_data_o <= o_data; ay_req <= ~ay_req; end
        // SD
        // -- Команда для чтения или записи на SD (1=R/W cmd)
        else if (A[7:0] == 8'h0F) begin sd_signal <= 1'b1; sd_cmd <= 1;      sd_out <= o_data; end
        // -- Отсылка команды [CMD=0 80T, 1=RW, 2=CS0, 3=CS1]
        else if (A[7:0] == 8'h1F) begin sd_signal <= 1'b1; sd_cmd <= o_data; sd_out <= 8'hFF; end
        // http://speccy.info/Порт_1FFD
        else if (A == 16'h1FFD) begin /* пустота */ end
        // Запись страницы 7FFDh (упрощенная дешифрация)
        else if (A[7:0] == 8'hFD) begin

            // Управлять памятью можно пока D5=0
            if (port7ffd[5] == 1'b0)
                port7ffd <= o_data;

        end
        // Запись бордера и звука
        else if (A[0] == 1'b0) begin spkr <= ^o_data[4:3]; border[2:0] <= o_data[2:0]; end

    end

end

endmodule
