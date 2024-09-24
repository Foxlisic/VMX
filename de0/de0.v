module de0
(
    // Reset
    input              RESET_N,

    // Clocks
    input              CLOCK_50,
    input              CLOCK2_50,
    input              CLOCK3_50,
    inout              CLOCK4_50,

    // DRAM
    output             DRAM_CKE,
    output             DRAM_CLK,
    output      [1:0]  DRAM_BA,
    output      [12:0] DRAM_ADDR,
    inout       [15:0] DRAM_DQ,
    output             DRAM_CAS_N,
    output             DRAM_RAS_N,
    output             DRAM_WE_N,
    output             DRAM_CS_N,
    output             DRAM_LDQM,
    output             DRAM_UDQM,

    // GPIO
    inout       [35:0] GPIO_0,
    inout       [35:0] GPIO_1,

    // 7-Segment LED
    output      [6:0]  HEX0,
    output      [6:0]  HEX1,
    output      [6:0]  HEX2,
    output      [6:0]  HEX3,
    output      [6:0]  HEX4,
    output      [6:0]  HEX5,

    // Keys
    input       [3:0]  KEY,

    // LED
    output      [9:0]  LEDR,

    // PS/2
    inout              PS2_CLK,
    inout              PS2_DAT,
    inout              PS2_CLK2,
    inout              PS2_DAT2,

    // SD-Card
    output             SD_CLK,
    inout              SD_CMD,
    inout       [3:0]  SD_DATA,

    // Switch
    input       [9:0]  SW,

    // VGA
    output      [3:0]  VGA_R,
    output      [3:0]  VGA_G,
    output      [3:0]  VGA_B,
    output             VGA_HS,
    output             VGA_VS
);

// Z-state
assign DRAM_DQ = 16'hzzzz;
assign GPIO_0  = 36'hzzzzzzzz;
assign GPIO_1  = 36'hzzzzzzzz;

// LED OFF
assign HEX0 = 7'b1111111;
assign HEX1 = 7'b1111111;
assign HEX2 = 7'b1111111;
assign HEX3 = 7'b1111111;
assign HEX4 = 7'b1111111;
assign HEX5 = 7'b1111111;

assign LEDR[0]    = ~mic;
assign SD_DATA[0] = 1'bZ;

// ---------------------------------------------------------------------
// Объявления проводов
// ---------------------------------------------------------------------

assign GPIO_1[35] = 1'b0;
assign GPIO_1[29] = spkr;

// Частоты
wire        locked;
reg         reset_n;
wire        clock_25;
wire        clock_50;
wire        clock_100;

// Контроллер
wire [7:0]  i_data;
wire [7:0]  o_data;
wire [15:0] address;
wire        we;
wire        m0;
wire        hold;
wire [ 7:0] portin;
wire        portrd;
wire        portwe;
wire        ram_we;
wire        vidpage;

// Память
wire [16:0] ram_address;
wire [14:0] rom_address;
wire [ 7:0] rom_idata, ram_idata;

// Видеоадаптер
wire [12:0] vaddr;
wire [16:0] addrhi;
wire [ 7:0] datahi, vdata;
wire [ 2:0] border;
wire        irq;

// Клавиатура
wire [7:0]  kbd;
wire [7:0]  ps2data;
wire        ps2hit;
wire        spkr;

// "Магнитная лента"
wire        mic;

// AY-чип
wire [ 3:0] ay_reg;
wire [ 7:0] ay_data_o;
wire [ 7:0] ay_data_i;
wire        ay_req;

// Интерфейс SD
wire [1:0]  sd_cmd;
wire [7:0]  sd_din;
wire [7:0]  sd_out;
wire        sd_signal;
wire        sd_busy;
wire        sd_timeout;

// Физически
// ---------------------------------------------------------------------
// Генератор частоты
// ---------------------------------------------------------------------

de0pll unit_pll
(
    .clkin     (CLOCK_50),
    .m25       (clock_25),
    .m50       (clock_50),
    .m100      (clock_100),
    .locked    (locked)
);

wire clock_cpu = clock_25;

always @(posedge clock_cpu) reset_n <= locked & RESET_N;

// Делитель частоты
clockdiv ClockDivUnit
(
    .reset_n    (reset_n),
    .clock      (clock_cpu),
    .active     (1'b1),
    .freq       (43),           // 7/50=3.5 Mhz; 4.3Mhz = 43/250
    .fref       (250),
    .hold       (hold)
);

// ---------------------------------------------------------------------
// Центральный процессор
// -----------------------------------------------------------------------------

z80 Z80Tb
(
    // Основной интерфейс
    .clock      (clock_cpu),
    .reset_n    (reset_n),
    .compat     (1'b1),         // =0 Быстрый режим =1 Совместимый
    .hold       (hold),         // =1 Активен =0 Ожидание
    .irq        (irq),
    .address    (address),
    .i_data     (i_data),
    .o_data     (o_data),
    .we         (we),
    .m0         (m0),
    .portin     (portin),
    .portwe     (portwe),
    .portrd     (portrd)
);

// ---------------------------------------------------------------------
// Видеоадаптер
// ---------------------------------------------------------------------

ula ULAUnit
(
    // Физический интерфейс
    .clock      (clock_25),
    .VGA_R      (VGA_R),
    .VGA_G      (VGA_G),
    .VGA_B      (VGA_B),
    .HS         (VGA_HS),
    .VS         (VGA_VS),
    // Обращение в память
    .port7ffd   (port7ffd),
    .vaddr      (vaddr),
    .vdata      (vdata),
    .addrhi     (addrhi),
    .datahi     (datahi),
    .border     (border),
    .sync50     (1'b0),        // Выбор 50/60 Гц SW[1]
    .irq        (irq)
);

// ---------------------------------------------------------------------
// Контроллер памяти и ресурсов
// -----------------------------------------------------------------------------

mmap ResourceRouterUnit
(
    // Подключение к процессору
    .reset_n        (reset_n),
    .clock          (clock_cpu),
    .m0             (m0),
    .hold           (hold),
    .address        (address),
    .o_data         (o_data),
    .i_data         (i_data),
    .we             (we),
    .portin         (portin),
    .portrd         (portrd),
    .portwe         (portwe),

    // ROM: 0=BASIC128,1=BASIC48
    .rom_address    (rom_address),
    .rom_idata      (rom_idata),

    // Запись или чтение из 128k памяти
    .ram_address    (ram_address),
    .ram_idata      (ram_idata),
    .ram_we         (ram_we),

    // Видеостраница
    .vidpage        (vidpage),
    .border         (border),

    // "Магнитная лента" и клавиатура
    .kbd            (kbd),
    .mic            (mic),
    .spkr           (spkr),

    // AY
    .ay_reg         (ay_reg),
    .ay_data_o      (ay_data_o),
    .ay_data_i      (ay_data_i),
    .ay_req         (ay_req),

    // SD-интерфейс
    .sd_signal      (sd_signal),   // In   =1 Сообщение отослано на spi
    .sd_cmd         (sd_cmd),      // In      Команда
    .sd_din         (sd_din),      // Out     Принятое сообщение от карты
    .sd_out         (sd_out),      // In      Сообщение на отправку к карте
    .sd_busy        (sd_busy),     // Out  =1 Занято
    .sd_timeout     (sd_timeout)   // Out  =1 Таймаут
);

// ---------------------------------------------------------------------
// Блоки памяти, 304k
// ---------------------------------------------------------------------

// 32k 0: 128k; 1: 48k BASIC
rom32 UnitRom32
(
    .clock      (clock_100),
    .address_a  (rom_address),
    .q_a        (rom_idata)
);

// 128k RAM
ram128 UnitRam128
(
    // Доступ из контроллера
    .clock      (clock_100),
    .address_a  (ram_address),
    .q_a        (ram_idata),
    .data_a     (o_data),
    .wren_a     (ram_we),

    // Доступ из видеоадаптера
    .address_b  ({1'b1, vidpage, 2'b10, vaddr[12:0]}),
    .q_b        (vdata)
);

// ---------------------------------------------------------------------
// Клавиатура
// ---------------------------------------------------------------------

// Контроллер PS/2
keyboard KeybUnit
(
    .CLOCK_50           (clock_50),     // Тактовый генератор на 50 Мгц
    .PS2_CLK            (PS2_CLK),      // Таймингс PS/2
    .PS2_DAT            (PS2_DAT),      // Данные с PS/2
    .received_data      (ps2data),      // Принятые данные
    .received_data_en   (ps2hit)        // Нажата клавиша
);

// Контроллер клавиатуры
kbd KbdControllerUnit
(
    .reset_n    (reset_n),
    .clock_50   (clock_50),
    .ps2data    (ps2data),
    .ps2hit     (ps2hit),
    .A          (address),
    .D          (kbd)
);

// Контроллер SD
// -----------------------------------------------------------------------------

sd UnitSD
(
    // 50 Mhz
    .clock50    (clock_50),

    // Физический интерфейс
    .SPI_CS     (SD_DATA[3]),   // Выбор чипа
    .SPI_SCLK   (SD_CLK),       // Тактовая частота
    .SPI_MISO   (SD_DATA[0]),   // Входящие данные
    .SPI_MOSI   (SD_CMD),       // Исходящие

    // Интерфейс
    .sd_signal  (sd_signal),    // In   =1 Сообщение отослано на spi
    .sd_cmd     (sd_cmd),       // In      Команда
    .sd_din     (sd_din),       // Out     Принятое сообщение от карты
    .sd_out     (sd_out),       // In      Сообщение на отправку к карте
    .sd_busy    (sd_busy),      // Out  =1 Занято
    .sd_timeout (sd_timeout)    // Out  =1 Таймаут
);

endmodule

`include "../app/z80.v"
