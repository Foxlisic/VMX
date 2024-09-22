#include <SDL2/SDL.h>

enum SpecModel {
    SPECTRUM_SINCLAIR = 1,
    SPECTRUM_PENTAGON = 2,
};

// Цвета Спектрума
static const uint32_t colors[16] =
{
    0x000000, 0x0000BB, 0xBB0000, 0xBB00BB,
    0x00BB00, 0x00BBBB, 0xBBBB00, 0xBBBBBB,
    0x000000, 0x0000FF, 0xFF0000, 0xFF00FF,
    0x00FF00, 0x00FFFF, 0xFFFF00, 0xFFFFFF
};

// 96 символов по 4x8
const uint8_t font4[384] = {
    0x00,0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00,0xAA,0xAE,0x0A,0x0A,0x0E,0x0A,0x00,
    0x40,0x4A,0xA2,0x44,0x24,0xA8,0x4A,0x40,0x40,0xA4,0x44,0xA0,0xA0,0xC0,0x60,0x00,
    0x00,0x28,0x44,0x44,0x44,0x44,0x28,0x00,0x00,0xA4,0x44,0xEE,0x44,0xA4,0x00,0x00,
    0x00,0x00,0x00,0x0E,0x00,0x40,0x40,0x80,0x00,0x02,0x02,0x04,0x04,0x48,0x48,0x00,
    0x00,0x44,0xAC,0xE4,0xA4,0xA4,0x4E,0x00,0x00,0x4E,0xA2,0x24,0x42,0x8A,0xE4,0x00,
    0x00,0x2E,0x68,0xAC,0xE2,0x2A,0x24,0x00,0x00,0x6E,0x82,0xC2,0xA4,0xA4,0x44,0x00,
    0x00,0x44,0xAA,0x4A,0xA6,0xA2,0x4C,0x00,0x00,0x00,0x44,0x00,0x00,0x44,0x08,0x00,
    0x00,0x00,0x2E,0x40,0x8E,0x40,0x20,0x00,0x00,0x04,0x8A,0x42,0x24,0x40,0x84,0x00,
    0x00,0x44,0xAA,0xAA,0x8E,0x8A,0x6A,0x00,0x00,0xC4,0xAA,0xC8,0xA8,0xAA,0xC4,0x00,
    0x00,0xCE,0xA8,0xAE,0xA8,0xA8,0xCE,0x00,0x00,0xE4,0x8A,0xE8,0x8A,0x8A,0x84,0x00,
    0x00,0xAE,0xA4,0xE4,0xA4,0xA4,0xAE,0x00,0x00,0x2A,0x2A,0x2C,0x2A,0xAA,0x4A,0x00,
    0x00,0x8A,0x8E,0x8A,0x8A,0x8A,0xEA,0x00,0x00,0x24,0xAA,0xEA,0xEA,0xAA,0x84,0x00,
    0x00,0xC4,0xAA,0xAA,0xCA,0x8C,0x86,0x00,0x00,0xC4,0xAA,0xA4,0xC2,0xAA,0xA4,0x00,
    0x00,0xEA,0x4A,0x4A,0x4A,0x4A,0x4E,0x00,0x00,0xAA,0xAA,0xAA,0xAA,0xAE,0x4A,0x00,
    0x00,0xAA,0xAA,0x44,0xA4,0xA4,0xA4,0x00,0x00,0xE6,0x24,0x44,0x44,0x84,0xE6,0x00,
    0x00,0x8C,0x84,0x44,0x44,0x24,0x2C,0x00,0x00,0x40,0xA0,0x00,0x00,0x00,0x00,0x0F,
    0x00,0x80,0x4C,0x02,0x06,0x0A,0x06,0x00,0x00,0x80,0x86,0xC8,0xA8,0xA8,0xC6,0x00,
    0x00,0x20,0x24,0x6A,0xAE,0xA8,0x66,0x00,0x00,0x20,0x46,0xEA,0x4A,0x46,0x42,0x0C,
    0x00,0x84,0x80,0xCC,0xA4,0xA4,0xAE,0x00,0x00,0x28,0x08,0x6A,0x2C,0x2A,0x2A,0xC0,
    0x00,0x40,0x4A,0x4E,0x4A,0x4A,0x4A,0x00,0x00,0x00,0xC4,0xAA,0xAA,0xAA,0xA4,0x00,
    0x00,0x00,0xC6,0xAA,0xAA,0xC6,0x82,0x82,0x00,0x00,0x66,0x88,0x84,0x82,0x8C,0x00,
    0x00,0x40,0x4A,0xEA,0x4A,0x4A,0x4E,0x00,0x00,0x00,0xAA,0xAA,0xAA,0xAE,0x4A,0x00,
    0x00,0x00,0xAA,0xAA,0x4A,0xA6,0xA2,0x0C,0x02,0x04,0xE4,0x28,0x44,0x84,0xE2,0x00,
    0x48,0x44,0x44,0x02,0x44,0x44,0x48,0x00,0x00,0x00,0xC0,0x60,0x00,0x00,0x00,0x00
};

const char* ds_mnemonics[256] = {

    /* 00 */    "nop",  "ld",   "ld",   "inc",  "inc",  "dec",  "ld",   "rlca",
    /* 08 */    "ex",   "add",  "ld",   "dec",  "inc",  "dec",  "ld",   "rrca",
    /* 10 */    "djnz", "ld",   "ld",   "inc",  "inc",  "dec",  "ld",   "rla",
    /* 18 */    "jr",   "add",  "ld",   "dec",  "inc",  "dec",  "ld",   "rra",
    /* 20 */    "jr",   "ld",   "ld",   "inc",  "inc",  "dec",  "ld",   "daa",
    /* 28 */    "jr",   "add",  "ld",   "dec",  "inc",  "dec",  "ld",   "cpl",
    /* 30 */    "jr",   "ld",   "ld",   "inc",  "inc",  "dec",  "ld",   "scf",
    /* 38 */    "jr",   "add",  "ld",   "dec",  "inc",  "dec",  "ld",   "cpl",

    /* 40 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",
    /* 48 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",
    /* 50 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",
    /* 58 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",
    /* 60 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",
    /* 68 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",
    /* 70 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "halt",  "ld",
    /* 78 */    "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",   "ld",

    /* 80 */    "add",  "add",  "add",  "add",  "add",  "add",  "add",  "add",
    /* 88 */    "adc",  "adc",  "adc",  "adc",  "adc",  "adc",  "adc",  "adc",
    /* 90 */    "sub",  "sub",  "sub",  "sub",  "sub",  "sub",  "sub",  "sub",
    /* 98 */    "sbc",  "sbc",  "sbc",  "sbc",  "sbc",  "sbc",  "sbc",  "sbc",
    /* A0 */    "and",  "and",  "and",  "and",  "and",  "and",  "and",  "and",
    /* A8 */    "xor",  "xor",  "xor",  "xor",  "xor",  "xor",  "xor",  "xor",
    /* B0 */    "or",   "or",   "or",   "or",   "or",   "or",   "or",   "or",
    /* B8 */    "cp",   "cp",   "cp",   "cp",   "cp",   "cp",   "cp",   "cp",

    /* C0 */    "ret",   "pop", "jp",   "jp",   "call", "push", "add",  "rst",
    /* C8 */    "ret",   "ret", "jp",   "$",    "call", "call", "adc",  "rst",
    /* D0 */    "ret",   "pop", "jp",   "out",  "call", "push", "sub",  "rst",
    /* D8 */    "ret",   "exx", "jp",   "in",   "call", "$",    "sbc",  "rst",
    /* E0 */    "ret",   "pop", "jp",   "ex",   "call", "push", "and",  "rst",
    /* E8 */    "ret",   "jp",  "jp",   "ex",   "call", "$",    "xor",  "rst",
    /* F0 */    "ret",   "pop", "jp",   "di",   "call", "push", "or",   "rst",
    /* F8 */    "ret",   "ld",  "jp",   "ei",   "call", "$",    "cp",   "rst"
};

const char* ds_reg8[3*8] = {
    "b", "c", "d", "e", "h",   "l",   "(hl)", "a",
    "b", "c", "d", "e", "ixh", "ixl", "$",    "a",
    "b", "c", "d", "e", "iyh", "iyl", "$",    "a"
};

const char* ds_reg16[3*4] = {

    "bc", "de", "hl", "sp",
    "bc", "de", "ix", "sp",
    "bc", "de", "iy", "sp"
};

const char* ds_reg16af[3*4] = {

    "bc", "de", "hl", "af",
    "bc", "de", "ix", "af",
    "bc", "de", "iy", "af"
};

const char* ds_cc[8] = {

    "nz", "z", "nc", "c",
    "po", "pe", "p", "m"
};

const char* ds_bits[8] = {

    "rlc", "rrc", "rl", "rr",
    "sla", "sra", "sll", "srl",
};

const int ds_im[8] = {0, 0, 1, 2, 0, 0, 1, 2};

class VMX
{
protected:

    SDL_Surface*        screen_surface;
    SDL_Window*         sdl_window;
    SDL_Renderer*       sdl_renderer;
    SDL_PixelFormat*    sdl_pixel_format;
    SDL_Texture*        sdl_screen_texture;
    SDL_Event           evt;
    Uint32*             screen_buffer;

    // Обработка фрейма
    int width, height, scale, length, pticks, ppu_x = 0, ppu_y = 0;

    uint8_t     rom[65536];
    uint8_t     ram[131072];
    uint16_t    lutfb[192];
    uint8_t     key_states[8];

    uint8_t     screen[352][312];
    uint8_t     port_7ffd, border, flash_state = 0;
    uint8_t     trdos_latch = 0, port_fe = 0;

    uint8_t     debug_console = 0;
    uint8_t     debug_window  = 0;
    uint16_t    debug_addr    = 0x0000;
    uint16_t    debug_dump    = 0x0000;

    uint8_t     cpu_halt = 0;
    uint8_t     compat = 1;
    uint8_t     spectrum_model = SPECTRUM_PENTAGON;

    // Disassembly.2000
    int     ds_ad;             // Текущая временная позиция разбора инструкции
    int     ds_size;           // Размер инструкции
    char    ds_opcode[16];
    char    ds_operand[32];
    char    ds_prefix[16];
    int     ds_string[64];     // Адреса в строках
    int     ds_change[16];

    int     locx = 320, locy = 0, fr = 0x000000, bg = 0x008080;

    Vz80* core;

public:

    VMX(int argc, char** argv)
    {
        scale       = 2;                // Удвоение пикселей
        width       = 320;              // Ширина экрана
        height      = 240;              // Высота экрана
        length      = (1000/50);        // 50 FPS
        pticks      = 0;
        port_7ffd   = 0x10;             // 48=0x10 128=0x00

        // Загрузка ROM
        for (int i = 0; i < 16384; i++) {

            rom[0x0000 + i] = rom128k[i];
            rom[0x4000 + i] = rom48k[i];
            rom[0x8000 + i] = trdosrom[i];
        }

        for (int i = 0; i < 8; i++) {
            key_states[i] = 0xFF;
        }

        // Заполнение таблицы адресов
        for (int y = 0; y < 192; y++) {
            lutfb[y] = 32*((y & 0x38)>>3) + 256*(y&7) + 2048*(y>>6);
        }

        int n = 1;

        // Разбор входящих параметров
        while (n < argc) {

            if (argv[n][0] == '-') {

                switch (argv[n][1])
                {
                    case '2': port_7ffd = 0x00; break;
                    case 'd': debug_console = 1; break;
                    case 'D': debug_window  = 1; width = 640; cpu_halt = 1; break;
                    case 'u': compat = 0; break;
                    case 'r': loadrom(argv[n+1], argv[n][2] - '0'); n++; break;
                }
            }

            n++;
        }

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            exit(1);
        }

        SDL_ClearError();
        sdl_window          = SDL_CreateWindow("ZXSpectrum Verilated Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width, scale * height, SDL_WINDOW_SHOWN);
        sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
        screen_buffer       = (Uint32*) malloc(width * height * sizeof(Uint32));
        sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

        // Перезапуск процессора
        core = new Vz80;
        reset(0);

        // Нарисовать окно отладчика
        if (debug_window) disasm_repaint();
    }

    void reset(int reloadreg)
    {
        // Загрузка нулевых регистров
        if (reloadreg == 0) {

            core->_bc       = 0;
            core->_de       = 0;
            core->_hl       = 0;
            core->_af       = 0xFFFF;
            core->_bc_prime = 0;
            core->_de_prime = 0;
            core->_hl_prime = 0;
            core->_ix       = 0;
            core->_iy       = 0;
            core->_pc       = 0;
            core->_sp       = 0xFFFF;
            core->_ir       = 0;
            core->_i_mode   = 0;
            core->_iff1     = 0;
            core->_iff2     = 0;
        }

        core->reset_n   = 0;
        core->hold      = 1;
        core->compat    = compat;
        core->irq       = 0;
        core->clock     = 0; core->eval();
        core->clock     = 1; core->eval();
        core->reset_n   = 1;
    }

    // Основной цикл работы
    int main()
    {
        SDL_Rect dstRect;

        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.w = scale * width;
        dstRect.h = scale * height;

        for (;;) {

            int cycles = 0;
            Uint32 start = SDL_GetTicks();

            // Прием событий
            while (SDL_PollEvent(& evt)) {

                // Событие выхода
                switch (evt.type) {

                    case SDL_QUIT: return 0;
                    case SDL_KEYDOWN:

                        kbd_scancode(evt.key.keysym.scancode, 1);
                        break;

                    case SDL_KEYUP:

                        kbd_scancode(evt.key.keysym.scancode, 0);
                        break;
                }
            }

            // Исполнение ЛЮБЫХ действий только при запущенном процессоре
            if (cpu_halt == 0) {

                ppu_x = ppu_y = 0;
                uint8_t brk = 0;

                // 69888=Sinclair Standart Cycles
                // 71680=Pentagonum
                // Длина строки у каждого равна 224
                int cycles_max = (spectrum_model == SPECTRUM_PENTAGON) ? 71680 : 69888;

                // Один кадр
                do {

                    for (int i = 0; i < 1024; i++) {

                        // Процессор остановлен
                        if (cpu_halt) { brk = 1; break; }

                        // Серия отладочных данных в консоли
                        if (core->m0 && debug_console) {
                            disasm(core->address);
                            printf("[%05d] #%04X %s %s\n", cycles, core->address, ds_opcode, ds_operand);
                        }

                        cpu_clock(cycles++);

                        // В режиме совместимости подсчет циклов
                        if (core->compat) {

                            ppu_clock();
                            ppu_clock();

                            if (cycles >= cycles_max) { brk = 1; break; }
                        }
                    }

                } while (SDL_GetTicks() - start < length && (brk == 0));

                // Отрендерить один фрейм в режиме несовместимости
                if (core->compat == 0) update_screen();

                // Мерцающие элементы
                flash_state = (flash_state + 1) % 50;
            }

            // В случае досрочного завершения кадра, ждать
            while (SDL_GetTicks() - start < length) SDL_Delay(1);

            // Обновление экрана
            SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, width * sizeof(Uint32));
            SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
            SDL_RenderClear         (sdl_renderer);
            SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, & dstRect);
            SDL_RenderPresent       (sdl_renderer);

            // Так как используется ПОЛНАЯ мощность, то слегка задержать кадр
            if (core->compat == 0) SDL_Delay(1);
        }
    }

    void cpu_clock(int cycles)
    {
        uint16_t A = core->address;

        // Чтение или запись в память
        if (core->we)     write(A, core->o_data);
        if (core->portwe) io_write(A, core->o_data);

        // Вход в TRDOS
        if (core->m0) trdos_handler(A);

        core->i_data = read(A);
        core->portin = io_read(A);

        // Инструкция HALT
        if (core->m0 && core->i_data == 0x76 && debug_window && !cpu_halt) {

            cpu_halt = 1;
            disasm_repaint();
            return;
        }

        // Запрос #IRQ, в compat режиме вызывается после 192-й линии
        core->irq = (cycles >= 224*192 && cycles < (224+1)*192);

        core->clock = 0; core->eval();
        core->clock = 1; core->eval();
    }

    // Полное обновление экрана
    void update_screen()
    {
        ppu_x = ppu_y = 0;
        for (int i = 0; i < 2*69888; i++) ppu_clock();
    }

    // Отсчитать количество тактов PPU
    void ppu_clock()
    {
        // Реальные размеры
        int x = ppu_x - 96,
            y = ppu_y - 8;

        // Коррекция под экран 320x200
        int cx = x - 16,
            cy = y - 32;

        // Видимая область
        if (x >= 0 && y >= 0) {

            // Область рисования PAPER
            if (x >= 48 && x < 48+256 && y >= 56 && y < 56+192) {

                if ((x & 7) == 0) {

                    int A  = lutfb[y-56] + ((x-48) >> 3);
                    int Bs = 0x4000*(port_7ffd & 0x08 ? 7 : 5);

                    int byte    = ram[Bs + A];
                    int attr    = ram[Bs + 0x1800 + (A & 0x1F) + ((A & 0x1800) >> 3) + (A & 0xE0)];
                    int bgcolor = (attr & 0x38) >> 3;
                    int frcolor = (attr & 0x07) + ((attr & 0x40) >> 3);
                    int flash   = (attr & 0x80) ? 1 : 0;
                    int bright  = (attr & 0x40) ? 8 : 0;

                    for (int j = 0; j < 8; j++) {

                        uint8_t  pix = (byte & (0x80 >> j)) ? 1 : 0;
                        uint32_t clr = bright | ((flash ? (pix ^ (flash_state > 25 ? 1 : 0)) : pix) ? frcolor : bgcolor);

                        screen[x + j][y] = clr;
                        if (cx < 320) pset(cx + j, cy, colors[clr]);
                    }
                }

            } else {

                screen[x][y] = border & 7;
                if (cx < 320) pset(cx, cy, colors[border & 7]);
            }
        }

        // Размер кадра равен [448 x 312]
        ppu_y = (ppu_y + (ppu_x == 447));
        ppu_x = (ppu_x + 1) % 448;

        // Максимальное количество строк
        if (ppu_y == (spectrum_model == SPECTRUM_PENTAGON ? 320 : 312)) ppu_y = 0;
    }

    // Убрать окно из памяти
    int destroy()
    {
        free(screen_buffer);
        SDL_DestroyTexture(sdl_screen_texture);
        SDL_FreeFormat(sdl_pixel_format);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return 0;
    }

    // Установка точки
    void pset(int x, int y, Uint32 cl)
    {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }

        screen_buffer[width*y + x] = cl;
    }

    // 0x0000-0x3fff ROM
    // 0x4000-0x7fff BANK 2
    // 0x8000-0xbfff BANK 5
    // 0xc000-0xffff BANK 0..7
    int get_bank(int address, int show_bank_id = 0)
    {
        int bank = 0;
        switch (address & 0xC000) {

            case 0x0000: bank = (port_7ffd & 0x30) ? 1 : 0; break;
            case 0x4000: bank = 5; break;
            case 0x8000: bank = 2; break;
            case 0xC000: bank = (port_7ffd & 7); break;
        }

        return show_bank_id ? bank : bank*16384 + (address & 0x3FFF);
    }

    // Чтение байта
    int read(int address)
    {
        // Обращение к ROM 128k|48k (0 или 16384)
        if (address < 0x4000) {
            return trdos_latch ? rom[0x8000 + address] : rom[get_bank(address)];
        }

        return ram[get_bank(address)];
    }

    // Запись байта
    void write(int address, int data)
    {
        address &= 0xFFFF;
        if (address < 0x4000) return;

        ram[get_bank(address)] = data;
    }

    // Чтение из порта
    int io_read(int port)
    {
        // Порт #7FFD
        if (port == 0x7FFD) {
            return port_7ffd;
        }
        // else if (port == 0xFFFD) { return ay_register; }
        // else if (port == 0xBFFD) { return ay_regs[ay_register%15]; }
        // Чтение клавиатуры
        else if ((port & 1) == 0) {

            int result = 0xff;
            for (int row = 0; row < 8; row++) {
                if (!(port & (1 << (row + 8)))) {
                    result &= key_states[ row ];
                }
            }

            return result;
        }
        // Kempston Joystick
        else if ((port & 0x00E0) == 0x0000) {
            return 0x00;
        }

        return 0xFF;
    }

    // Запись в порт
    void io_write(int port, int data)
    {
        if (port == 0x7ffd) {

            // Не менять бит D5 если он 1
            if ((port_7ffd & 0x20) && (data & 0x20) == 0) {
                data |= 0x20;
            }

            port_7ffd = data;
        }
        // AY address register
        // else if (port == 0xFFFD) { ay_register = data & 15; }
        // AY address data
        // else if (port == 0xBFFD) { ay_write_data(data); }
        // https://speccy.info/Порт_1FFD
        else if (port == 0x1FFD) {
            /* ничего пока что */
        }
        else if ((port & 1) == 0) {

            border  = (data & 7);
            port_fe = data;
        }
    }

    // Загрузка регистров перед переустановкой CPU state
    void reloadRegisters()
    {
        core->_bc = core->bc; core->_bc_prime = core->bc_prime;
        core->_de = core->de; core->_de_prime = core->de_prime;
        core->_hl = core->hl; core->_hl_prime = core->hl_prime;
        core->_af = core->af; core->_af_prime = core->af_prime;
        core->_ix = core->ix;
        core->_iy = core->iy;
        core->_pc = core->pc;
        core->_sp = core->sp;
        core->_ir = core->ir;
        core->_i_mode = core->i_mode;
        core->_iff1   = core->iff1;
        core->_iff2   = core->iff2;
    }

    // Проверяется наличие входа и выхода из TRDOS
    void trdos_handler(uint16_t pc)
    {
        // Только 48k ROM разрешен
        if (port_7ffd & 0x10) {

            // Вход в TRDOS : инструкция находится в адресе 3Dh
            if      (!trdos_latch && (pc & 0xFF00) == 0x3D00) { trdos_latch = 1; }
            // Выход из TRDOS
            else if ( trdos_latch && (pc & 0xC000))           { trdos_latch = 0; }
        }
    }

    // Занесение нажатия в регистры
    void key_press(int row, int mask, int press)
    {
        if (press) {
            key_states[ row ] &= ~mask;
        } else {
            key_states[ row ] |=  mask;
        }
    }

    // Сканирование нажатой клавиши
    // https://ru.wikipedia.org/wiki/Скан-код
    void kbd_scancode(int scancode, int press) {

        switch (scancode) {

            // Коды клавиш A-Z
            case SDL_SCANCODE_A: key_press(1, 0x01, press); break;
            case SDL_SCANCODE_B: key_press(7, 0x10, press); break;
            case SDL_SCANCODE_C: key_press(0, 0x08, press); break;
            case SDL_SCANCODE_D: key_press(1, 0x04, press); break;
            case SDL_SCANCODE_E: key_press(2, 0x04, press); break;
            case SDL_SCANCODE_F: key_press(1, 0x08, press); break;
            case SDL_SCANCODE_G: key_press(1, 0x10, press); break;
            case SDL_SCANCODE_H: key_press(6, 0x10, press); break;
            case SDL_SCANCODE_I: key_press(5, 0x04, press); break;
            case SDL_SCANCODE_J: key_press(6, 0x08, press); break;
            case SDL_SCANCODE_K: key_press(6, 0x04, press); break;
            case SDL_SCANCODE_L: key_press(6, 0x02, press); break;
            case SDL_SCANCODE_M: key_press(7, 0x04, press); break;
            case SDL_SCANCODE_N: key_press(7, 0x08, press); break;
            case SDL_SCANCODE_O: key_press(5, 0x02, press); break;
            case SDL_SCANCODE_P: key_press(5, 0x01, press); break;
            case SDL_SCANCODE_Q: key_press(2, 0x01, press); break;
            case SDL_SCANCODE_R: key_press(2, 0x08, press); break;
            case SDL_SCANCODE_S: key_press(1, 0x02, press); break;
            case SDL_SCANCODE_T: key_press(2, 0x10, press); break;
            case SDL_SCANCODE_U: key_press(5, 0x08, press); break;
            case SDL_SCANCODE_V: key_press(0, 0x10, press); break;
            case SDL_SCANCODE_W: key_press(2, 0x02, press); break;
            case SDL_SCANCODE_X: key_press(0, 0x04, press); break;
            case SDL_SCANCODE_Y: key_press(5, 0x10, press); break;
            case SDL_SCANCODE_Z: key_press(0, 0x02, press); break;

            // Цифры
            case SDL_SCANCODE_1: key_press(3, 0x01, press); break;
            case SDL_SCANCODE_2: key_press(3, 0x02, press); break;
            case SDL_SCANCODE_3: key_press(3, 0x04, press); break;
            case SDL_SCANCODE_4: key_press(3, 0x08, press); break;
            case SDL_SCANCODE_5: key_press(3, 0x10, press); break;
            case SDL_SCANCODE_6: key_press(4, 0x10, press); break;
            case SDL_SCANCODE_7: key_press(4, 0x08, press); break;
            case SDL_SCANCODE_8: key_press(4, 0x04, press); break;
            case SDL_SCANCODE_9: key_press(4, 0x02, press); break;
            case SDL_SCANCODE_0: key_press(4, 0x01, press); break;

            // Специальные символы
            case SDL_SCANCODE_GRAVE:        key_press(0, 0x01, press); key_press(3, 0x01, press); break; // SS+1 EDIT
            case SDL_SCANCODE_MINUS:        key_press(7, 0x02, press); key_press(6, 0x08, press); break; // -
            case SDL_SCANCODE_EQUALS:       key_press(7, 0x02, press); key_press(6, 0x02, press); break; // =
            case SDL_SCANCODE_BACKSLASH:    break;
            case SDL_SCANCODE_LEFTBRACKET:  break;
            case SDL_SCANCODE_RIGHTBRACKET: break;
            case SDL_SCANCODE_SEMICOLON:    break;
            case SDL_SCANCODE_APOSTROPHE:   break;
            case SDL_SCANCODE_COMMA:        key_press(7, 0x02, press); key_press(7, 0x08, press); break; // ,
            case SDL_SCANCODE_PERIOD:       key_press(7, 0x02, press); key_press(7, 0x04, press); break; // .
            case SDL_SCANCODE_SLASH:        break;
            case SDL_SCANCODE_BACKSPACE:    key_press(0, 0x01, press); key_press(4, 0x01, press); break; // SS+0 BS
            case SDL_SCANCODE_SPACE:        key_press(7, 0x01, press); break;
            case SDL_SCANCODE_TAB:          key_press(0, 0x01, press); key_press(7, 0x02, press); break; //  SS+CS
            case SDL_SCANCODE_CAPSLOCK:     key_press(0, 0x01, press); key_press(3, 0x02, press); break; // SS+2 CAP (DANGER)
            case SDL_SCANCODE_LSHIFT:       key_press(0, 0x01, press); break;
            case SDL_SCANCODE_LCTRL:        break;
            case SDL_SCANCODE_LALT:         break;
            case SDL_SCANCODE_RSHIFT:       key_press(7, 0x02, press); break;
            case SDL_SCANCODE_RETURN:       key_press(6, 0x01, press); break;
            case SDL_SCANCODE_ESCAPE:       key_press(0, 0x01, press); key_press(7, 0x01, press); break; // SS+SPC
            case SDL_SCANCODE_NUMLOCKCLEAR: break;
            case SDL_SCANCODE_KP_DIVIDE:    key_press(7, 0x02, press); key_press(0, 0x10, press); break; // /
            case SDL_SCANCODE_KP_ENTER:     key_press(6, 0x01, press); break;
            case SDL_SCANCODE_KP_MULTIPLY:  key_press(7, 0x02, press); key_press(7, 0x10, press); break; // *
            case SDL_SCANCODE_KP_MINUS:     key_press(7, 0x02, press); key_press(6, 0x08, press); break; // -
            case SDL_SCANCODE_KP_PLUS:      key_press(7, 0x02, press); key_press(6, 0x04, press); break; // +
            case SDL_SCANCODE_KP_PERIOD:    break;
            case SDL_SCANCODE_SCROLLLOCK:   break;

            case SDL_SCANCODE_UP:           key_press(0, 0x01, press); key_press(4, 0x08, press); break; // SS+7
            case SDL_SCANCODE_DOWN:         key_press(0, 0x01, press); key_press(4, 0x10, press); break; // SS+6
            case SDL_SCANCODE_LEFT:         key_press(0, 0x01, press); key_press(3, 0x10, press); break; // SS+5
            case SDL_SCANCODE_RIGHT:        key_press(0, 0x01, press); key_press(4, 0x04, press); break; // SS+8

            // Keypad
            /*
            case SDL_SCANCODE_KP_0: if (release) kbd_push(0xF0); kbd_push(0x70); break;
            case SDL_SCANCODE_KP_1: if (release) kbd_push(0xF0); kbd_push(0x69); break;
            case SDL_SCANCODE_KP_2: if (release) kbd_push(0xF0); kbd_push(0x72); break;
            case SDL_SCANCODE_KP_3: if (release) kbd_push(0xF0); kbd_push(0x7A); break;
            case SDL_SCANCODE_KP_4: if (release) kbd_push(0xF0); kbd_push(0x6B); break;
            case SDL_SCANCODE_KP_5: if (release) kbd_push(0xF0); kbd_push(0x73); break;
            case SDL_SCANCODE_KP_6: if (release) kbd_push(0xF0); kbd_push(0x74); break;
            case SDL_SCANCODE_KP_7: if (release) kbd_push(0xF0); kbd_push(0x6C); break;
            case SDL_SCANCODE_KP_8: if (release) kbd_push(0xF0); kbd_push(0x75); break;
            case SDL_SCANCODE_KP_9: if (release) kbd_push(0xF0); kbd_push(0x7D); break;
            */

            case SDL_SCANCODE_F1:   if (press) disasm_repaint(); break;
            case SDL_SCANCODE_F2:   if (press) update_screen(); break;
            case SDL_SCANCODE_F7:   if (press) disasm_step(); break;
            case SDL_SCANCODE_F9:   if (press) disasm_detach(); break;

            // Клавиша PrnScr
            /*
            case SDL_SCANCODE_PRINTSCREEN: {

                if (release == 0) {

                    kbd_push(0xE0); kbd_push(0x12);
                    kbd_push(0xE0); kbd_push(0x7C);

                } else {

                    kbd_push(0xE0); kbd_push(0xF0); kbd_push(0x7C);
                    kbd_push(0xE0); kbd_push(0xF0); kbd_push(0x12);
                }

                break;
            }

            // Клавиша Pause
            case SDL_SCANCODE_PAUSE: {

                kbd_push(0xE1);
                kbd_push(0x14); if (release) kbd_push(0xF0); kbd_push(0x77);
                kbd_push(0x14); if (release) kbd_push(0xF0); kbd_push(0x77);
                break;
            }
            */
        }
    }

    // ====================== ОКНО ОТЛАДЧИКА ===========================

    // Выполнить один шаг дизассемблера
    void disasm_step()
    {
        if (debug_window == 0) return;

        // Состояние до
        uint16_t state1[16] = {
            core->bc, core->bc_prime,
            core->de, core->de_prime,
            core->hl, core->hl_prime,
            core->af, core->af_prime,
            core->ix,
            core->iy,
            core->pc,
            core->sp,
            (uint16_t)(core->ir & 0xFF00),
            core->i_mode,
            core->iff1,
            core->iff2,
        };

        cpu_halt = 1;

        // Сначала, дочитать такты процессора
        while (core->m0 == 0) { cpu_clock(0); ppu_clock(); ppu_clock(); };

        // Потом прочесть все такты дальше
        do { cpu_clock(0); ppu_clock(); ppu_clock(); } while (core->m0 == 0);

        // Инструкция HALT
        if (read(core->address) == 0x76) {

            reloadRegisters();
            core->_pc++;
            reset(1);
        }

        // Состояние после
        uint16_t state2[16] = {
            core->bc, core->bc_prime,
            core->de, core->de_prime,
            core->hl, core->hl_prime,
            core->af, core->af_prime,
            core->ix,
            core->iy,
            core->pc,
            core->sp,
            (uint16_t)(core->ir & 0xFF00),
            core->i_mode,
            core->iff1,
            core->iff2,
        };

        for (int i = 0; i < 16; i++) ds_change[i] = (state1[i] != state2[i]);

        disasm_repaint();
    }

    // Запуск программы
    void disasm_detach()
    {
        if (debug_window == 0) return;

        if (cpu_halt) {

            cpu_halt = 0;
            for (int y = 0; y < 240; y++)
            for (int x = y & 1; x < 320; x += 2)
                pset(320 + x, y, 0);

        } else {

            cpu_halt = 1;
            disasm_repaint();
        }
    }

    void loc(int x, int y)
    {
        locx = 320 + 4*x;
        locy = y*8;
    }

    void printch(int x, int y, int ch)
    {
        for (int i = 0; i < 8; i++) {

            uint8_t a = font4[8*((ch - 32) >> 1) + i];
            a = (ch & 1) ? a & 15 : a >> 4;

            for (int j = 0; j < 4; j++) {
                pset(x + j, y + i, a & (16 >> j) ? fr : bg);
            }
        }
    }

    void print(const char* s)
    {
        int i = 0;
        while (s[i])
        {
            printch(locx, locy, s[i]);
            locx += 4;
            if (locx > 640) { locx = 320; locy += 8; }
            i++;
        }
    }

    // Перерисовать дизассемблер
    void disasm_repaint()
    {
        char buf[256];
        char cht[9];

        int _black = 0x000000;
        int _white = 0xFFFFFF;
        int _blue  = 0x0000A0;
        int _cyan  = 0x00A0A0;

        bg = _cyan;

        // Очистка экрана в цвет
        for (int y = 0;   y <  320; y++) for (int x = 320; x < 640; x++) pset(x, y, _cyan);
        for (int x = 322; x <  638; x++) { pset(x,   2,  _white); pset(x, 238, _white); }
        for (int x = 324; x <  636; x++) { pset(x,   4,  _white); pset(x, 236, _white); }
        for (int y = 2;   y <= 238; y++) { pset(322, y,  _white); pset(638, y, _white); }
        for (int y = 4;   y <= 236; y++) { pset(324, y,  _white); pset(636, y, _white); pset(450, y, _white); pset(452, y, _white); }
        for (int x = 452; x <  636; x++) { pset(x,   98, _white); pset(x, 100, _white); }

        int a = debug_addr, b;
        int dsize, match = 0;

        do {

            // Вывести ассемблерный листинг
            for (int i = 0; i < 28; i++) {

                b = get_bank(a, 1);

                // Выделение текущей строки
                if (a == core->pc) { match = 1; fr = _white; bg = _blue; } else { fr = _black; bg = _cyan; }

                dsize = disasm(a, 18);
                loc(2, i + 1); sprintf(buf, "%c%d:%04X %s %s", ((a == core->pc) ? '#' : ' '), b, a, ds_opcode, ds_operand); print(buf);
                a += dsize;
            }

            if (match == 0) { a = debug_addr = core->pc; }

        } while (match == 0);


        bg = _cyan;

        loc(34, 1);
        fr = _black; print("BC ");
        fr = ds_change[0] ? _white : _black; sprintf(buf, "%04X ", core->bc); print(buf);
        fr = ds_change[1] ? _white : _black; sprintf(buf, "%04X ", core->bc_prime); print(buf);

        loc(34, 2);
        fr = _black; print("DE ");
        fr = ds_change[2] ? _white : _black; sprintf(buf, "%04X ", core->de); print(buf);
        fr = ds_change[3] ? _white : _black; sprintf(buf, "%04X ", core->de_prime); print(buf);

        loc(34, 3);
        fr = _black; print("HL ");
        fr = ds_change[4] ? _white : _black; sprintf(buf, "%04X ", core->hl); print(buf);
        fr = ds_change[5] ? _white : _black; sprintf(buf, "%04X ", core->hl_prime); print(buf);

        loc(34, 4);
        fr = _black; print("AF ");
        fr = ds_change[6] ? _white : _black; sprintf(buf, "%04X ", core->af); print(buf);
        fr = ds_change[7] ? _white : _black; sprintf(buf, "%04X ", core->af_prime); print(buf);

        loc(34, 5);  fr = _black; print("IX "); fr = ds_change[8] ? _white : _black; sprintf(buf, "%04X", core->ix); print(buf);
        loc(34, 6);  fr = _black; print("IY "); fr = ds_change[9] ? _white : _black; sprintf(buf, "%04X", core->iy); print(buf);
        loc(34, 7);  fr = _black; print("PC "); fr = ds_change[10] ? _white : _black; sprintf(buf, "%04X", core->pc); print(buf);
        loc(34, 8);  fr = _black; print("SP "); fr = ds_change[11] ? _white : _black; sprintf(buf, "%04X", core->sp); print(buf);
        loc(34, 9);  fr = _black; print("IR "); fr = ds_change[12] ? _white : _black; sprintf(buf, "%04X", core->ir); print(buf);
        loc(34, 10); fr = _black; print("IMODE "); fr = ds_change[13] ? _white : _black; sprintf(buf, "%d", core->i_mode); print(buf);
        loc(34, 11); fr = _black; print("IFF12 "); fr = ds_change[14] || ds_change[15] ? _white : _black; sprintf(buf, "%d %d", core->iff1, core->iff2); print(buf);

        fr = _black;

        // Дамп
        for (int i = 0; i < 16; i++) {

            a = debug_dump + 8*i;
            b = get_bank(a, 1);

            loc(34, i + 13); sprintf(buf, "%d:%04X ", b, a); print(buf);
            for (int j = 0; j < 8; j++) {

                int t = read(a + j);
                sprintf(buf, "%02X ", t);
                print(buf);

                cht[j] = t >= 0x20 && t <= 0x7F ? t : ' ';
            }

            cht[8] = 0; print(cht);
        }

        // Стек
        for (int i = 0; i < 11; i++) {

            uint16_t sp = core->sp + i*2;

            loc(73, i + 1);
            sprintf(buf, "%c%04X", (sp == core->sp ? '>' : ' '), read(sp) + 256*read(sp+1)); print(buf);
        }

        // Вертикальные обрамляющие полоски
        for (int y = 4; y < 98; y++)    { pset(608, y, _white); pset(610, y, _white); }
        for (int y = 100; y < 236; y++) { pset(615, y, _white); pset(617, y, _white); }

        // Вывод состояния флагов
        loc(75, 13); print(core->af & 0x8000 ? "S 1" : "s .");
        loc(75, 14); print(core->af & 0x4000 ? "Z 1" : "z .");
        loc(75, 15); print(core->af & 0x2000 ? "5 1" : "5 .");
        loc(75, 16); print(core->af & 0x1000 ? "H 1" : "h .");
        loc(75, 17); print(core->af & 0x0800 ? "3 1" : "3 .");
        loc(75, 18); print(core->af & 0x0400 ? "P 1" : "p .");
        loc(75, 19); print(core->af & 0x0200 ? "N 1" : "n .");
        loc(75, 20); print(core->af & 0x0100 ? "C 1" : "c .");

        // Текущее положение луча
        loc(75, 22); print("ULA");
        loc(75, 23); sprintf(buf, "%03d", ppu_x); print(buf);
        loc(75, 24); sprintf(buf, "%03d", ppu_y); print(buf);

        // Подписи
        fr = _white;
        bg = _blue;  loc(2,   0); print(" Asm ");
        bg = _cyan;  loc(34,  0); print(" Reg ");
        bg = _cyan;  loc(34, 12); print(" Dump ");
        bg = _cyan;  loc(73,  0); print(" Stck ");
        bg = _cyan;  loc(75, 12); print(" F ");
    }

    // ====================== DISASSEMBY 2000 ==========================

    // Сформировать операнд (IX|IY+d)
    void ixy_disp(int prefix)
    {
        int df = ds_fetch_byte();

        if (df & 0x80) {
            sprintf(ds_prefix, "(%s-$%02x)", (prefix == 1 ? "ix" : "iy"), 1 + (df ^ 0xff));
        } else if (df) {
            sprintf(ds_prefix, "(%s+$%02x)", (prefix == 1 ? "ix" : "iy"), df);
        } else {
            sprintf(ds_prefix, "(%s)", (prefix == 1 ? "ix" : "iy"));
        }
    }

    // Прочитать байт дизассемблера
    int ds_fetch_byte()
    {
        int b = read(ds_ad);
        ds_ad = (ds_ad + 1) & 0xffff;
        ds_size++;
        return b;
    }

    // Прочитать слово дизассемблера
    int ds_fetch_word()
    {
        int l = ds_fetch_byte();
        int h = ds_fetch_byte();
        return (h << 8) | l;
    }

    // Прочитать относительный операнд
    int ds_fetch_rel()
    {
        int r8 = ds_fetch_byte();
        return ((r8 & 0x80) ? r8 - 0x100 : r8) + ds_ad;
    }

    // Дизассемблирование 1 линии
    int disasm(int addr, int oppad = 0)
    {
        int op, df;
        int prefix = 0;

        ds_opcode[0]  = 0;
        ds_operand[0] = 0;
        ds_prefix[0]  = 0;
        ds_ad   = addr;
        ds_size = 0;

        // -----------------------------------------------------------------
        // Считывание опкода и префиксов
        // -----------------------------------------------------------------

        do {

            op = ds_fetch_byte();
            if (op == 0xDD)      prefix = 1;
            else if (op == 0xFD) prefix = 2;
        }
        while (op == 0xDD || op == 0xFD);

        // -----------------------------------------------------------------
        // Разбор опкода и операндов
        // -----------------------------------------------------------------

        if (op == 0xED) {

            op = ds_fetch_byte();

            int a = (op & 0x38) >> 3;
            int b = (op & 0x07);
            int f = (op & 0x30) >> 4;

            // 01xx x000
            if      ((op & 0xc7) == 0x40) { sprintf(ds_opcode, "in");  sprintf(ds_operand, "%s, (c)", a == 6 ? "?" : ds_reg8[a]); }
            else if ((op & 0xc7) == 0x41) { sprintf(ds_opcode, "out"); sprintf(ds_operand, "(c), %s", a == 6 ? "0" : ds_reg8[a]); }
            // 01xx x010
            else if ((op & 0xc7) == 0x42) { sprintf(ds_opcode, op & 8 ? "adc" : "sbc"); sprintf(ds_operand, "hl, %s", ds_reg16[f]); }
            // 01xx b011
            else if ((op & 0xcf) == 0x43) { sprintf(ds_opcode, "ld"); sprintf(ds_operand, "($%04x), %s", ds_fetch_word(), ds_reg16[f]); }
            else if ((op & 0xcf) == 0x4b) { sprintf(ds_opcode, "ld"); sprintf(ds_operand, "%s, ($%04x)", ds_reg16[f], ds_fetch_word()); }
            // 01xx x10b
            else if ((op & 0xc7) == 0x44) { sprintf(ds_opcode, "neg"); }
            else if (op == 0x4d) sprintf(ds_opcode, "reti");
            else if ((op & 0xc7) == 0x45) { sprintf(ds_opcode, "retn"); }
            // 01xx x110
            else if ((op & 0xc7) == 0x46) { sprintf(ds_opcode, "im"); sprintf(ds_operand, "%x", ds_im[a]); }
            else switch (op) {

                case 0x47: sprintf(ds_opcode, "ld"); sprintf(ds_operand, "i, a"); break;
                case 0x4f: sprintf(ds_opcode, "ld"); sprintf(ds_operand, "r, a"); break;
                case 0x57: sprintf(ds_opcode, "ld"); sprintf(ds_operand, "a, i"); break;
                case 0x5f: sprintf(ds_opcode, "ld"); sprintf(ds_operand, "a, r"); break;
                case 0x67: sprintf(ds_opcode, "rrd"); break;
                case 0x6f: sprintf(ds_opcode, "rld"); break;

                case 0xa0: sprintf(ds_opcode, "ldi"); break;
                case 0xa1: sprintf(ds_opcode, "cpi"); break;
                case 0xa2: sprintf(ds_opcode, "ini"); break;
                case 0xa3: sprintf(ds_opcode, "outi"); break;
                case 0xa8: sprintf(ds_opcode, "ldd"); break;
                case 0xa9: sprintf(ds_opcode, "cpd"); break;
                case 0xaa: sprintf(ds_opcode, "ind"); break;
                case 0xab: sprintf(ds_opcode, "outd"); break;

                case 0xb0: sprintf(ds_opcode, "ldir"); break;
                case 0xb1: sprintf(ds_opcode, "cpir"); break;
                case 0xb2: sprintf(ds_opcode, "inir"); break;
                case 0xb3: sprintf(ds_opcode, "otir"); break;
                case 0xb8: sprintf(ds_opcode, "lddr"); break;
                case 0xb9: sprintf(ds_opcode, "cpdr"); break;
                case 0xba: sprintf(ds_opcode, "indr"); break;
                case 0xbb: sprintf(ds_opcode, "otdr"); break;

                default:

                    sprintf(ds_opcode, "undef?"); break;
            }
        }
        else if (op == 0xCB) {

            if (prefix) ixy_disp(prefix);
            op = ds_fetch_byte();

            int a = (op & 0x38) >> 3;
            int b = (op & 0x07);

            // 00xxxrrr SHIFT
            if ((op & 0xc0) == 0x00) {

                sprintf(ds_opcode, "%s", ds_bits[a]);

                if (prefix && b == 6) {
                    sprintf(ds_operand, "%s", ds_prefix);
                } else {
                    sprintf(ds_operand, "%s", ds_reg8[b + prefix*8]);
                }
            }
            else {

                if ((op & 0xc0) == 0x40) sprintf(ds_opcode, "bit");
                if ((op & 0xc0) == 0x80) sprintf(ds_opcode, "res");
                if ((op & 0xc0) == 0xc0) sprintf(ds_opcode, "set");

                sprintf(ds_operand, "%x, %s", a, prefix ? ds_prefix : ds_reg8[b]);
            }

        } else {

            // Имя опкода
            sprintf(ds_opcode, "%s", ds_mnemonics[op]);

            int a = (op & 0x38) >> 3;
            int b = (op & 0x07);

            // Имя HL в зависимости от префикса
            char hlname[4];
            if (prefix == 0) sprintf(hlname, "hl");
            if (prefix == 1) sprintf(hlname, "ix");
            if (prefix == 2) sprintf(hlname, "iy");

            // Инструкции перемещения LD
            if (op >= 0x40 && op < 0x80) {

                if (a == 6 && b == 6) {
                    /* halt */
                }
                // Префиксированные
                else if (prefix) {

                    // Прочитать +disp8
                    ixy_disp(prefix);

                    // Декодирование
                    if (a == 6) {
                        sprintf(ds_operand, "%s, %s", ds_prefix, ds_reg8[b]);
                    } else if (b == 6) {
                        sprintf(ds_operand, "%s, %s", ds_reg8[a], ds_prefix);
                    } else {
                        sprintf(ds_operand, "%s, %s", ds_reg8[8*prefix + a], ds_reg8[8*prefix + b]);
                    }
                }
                else { sprintf(ds_operand, "%s, %s", ds_reg8[a], ds_reg8[b]); }
            }
            // Арифметико-логика
            else if (op >= 0x80 && op < 0xc0) {

                if (prefix) {

                    if (b == 6) {

                        ixy_disp(prefix);
                        sprintf(ds_operand, "%s", ds_prefix);

                    } else {
                        sprintf(ds_operand, "%s", ds_reg8[8*prefix + b]);
                    }
                } else {
                    sprintf(ds_operand, "%s", ds_reg8[b]);
                }
            }
            // LD r16, **
            else if (op == 0x01 || op == 0x11 || op == 0x21 || op == 0x31) {

                df = ds_fetch_word();
                sprintf(ds_operand, "%s, $%04x", ds_reg16[4*prefix + ((op & 0x30) >> 4)], df);
            }
            // 00xx x110 LD r8, i8
            else if ((op & 0xc7) == 0x06) {

                if (a == 6 && prefix) {
                    ixy_disp(prefix);
                    sprintf(ds_operand, "%s, $%02x", ds_prefix, ds_fetch_byte());
                } else {
                    sprintf(ds_operand, "%s, $%02x", ds_reg8[8*prefix + a], ds_fetch_byte());
                }
            }
            // 00_xxx_10x
            else if ((op & 0xc7) == 0x04 || (op & 0xc7) == 0x05) {

                if (a == 6 && prefix) {
                    ixy_disp(prefix);
                    sprintf(ds_operand, "%s", ds_prefix);
                } else {
                    sprintf(ds_operand, "%s", ds_reg8[8*prefix + a]);
                }
            }
            // 00xx x011
            else if ((op & 0xc7) == 0x03) {
                sprintf(ds_operand, "%s", ds_reg16[4*prefix + ((op & 0x30) >> 4)]);
            }
            // 00xx 1001
            else if ((op & 0xcf) == 0x09) {
                sprintf(ds_operand, "%s, %s", ds_reg16[4*prefix+2], ds_reg16[4*prefix + ((op & 0x30) >> 4)]);
            }
            else if (op == 0x02) sprintf(ds_operand, "(bc), a");
            else if (op == 0x08) sprintf(ds_operand, "af, af'");
            else if (op == 0x0A) sprintf(ds_operand, "a, (bc)");
            else if (op == 0x12) sprintf(ds_operand, "(de), a");
            else if (op == 0x1A) sprintf(ds_operand, "a, (de)");
            else if (op == 0xD3) sprintf(ds_operand, "($%02x), a", ds_fetch_byte());
            else if (op == 0xDB) sprintf(ds_operand, "a, ($%02x)", ds_fetch_byte());
            else if (op == 0xE3) sprintf(ds_operand, "(sp), %s", hlname);
            else if (op == 0xE9) sprintf(ds_operand, "(%s)", hlname);
            else if (op == 0xEB) sprintf(ds_operand, "de, %s", hlname);
            else if (op == 0xF9) sprintf(ds_operand, "sp, %s", hlname);
            else if (op == 0xC3 || op == 0xCD) sprintf(ds_operand, "$%04x", ds_fetch_word());
            else if (op == 0x22) { b = ds_fetch_word(); sprintf(ds_operand, "($%04x), %s", b, hlname); }
            else if (op == 0x2A) { b = ds_fetch_word(); sprintf(ds_operand, "%s, ($%04x)", hlname, b); }
            else if (op == 0x32) { b = ds_fetch_word(); sprintf(ds_operand, "($%04x), a", b); }
            else if (op == 0x3A) { b = ds_fetch_word(); sprintf(ds_operand, "a, ($%04x)", b); }
            else if (op == 0x10 || op == 0x18) { sprintf(ds_operand, "$%04x", ds_fetch_rel()); }
            // 001x x000 JR c, *
            else if ((op & 0xe7) == 0x20) sprintf(ds_operand, "%s, $%04x", ds_cc[(op & 0x18)>>3], ds_fetch_rel());
            // 11xx x000 RET *
            else if ((op & 0xc7) == 0xc0) sprintf(ds_operand, "%s", ds_cc[a]);
            // 11xx x010 JP c, **
            // 11xx x100 CALL c, **
            else if ((op & 0xc7) == 0xc2 || (op & 0xc7) == 0xc4) sprintf(ds_operand, "%s, $%04x", ds_cc[a], ds_fetch_word());
            // 11xx x110 ALU A, *
            else if ((op & 0xc7) == 0xc6) sprintf(ds_operand, "$%02x", ds_fetch_byte());
            // 11xx x111 RST #
            else if ((op & 0xc7) == 0xc7) sprintf(ds_operand, "$%02x", op & 0x38);
            // 11xx 0x01 PUSH/POP r16
            else if ((op & 0xcb) == 0xc1) sprintf(ds_operand, "%s", ds_reg16af[ ((op & 0x30) >> 4) + prefix*4 ] );
        }

        int ds_opcode_len  = strlen(ds_opcode);
        int ds_operand_len = strlen(ds_operand);
        int ds_opcode_pad  = ds_opcode_len < 5 ? 5 : ds_opcode_len + 1;
        int ds_operand_pad = oppad > ds_operand_len ? oppad : ds_operand_len;

        // Привести в порядок
        for (int i = 0; i <= ds_opcode_pad; i++) {
             ds_opcode[i] = i < ds_opcode_len ? toupper(ds_opcode[i]) : (i == ds_opcode_pad-1 ? 0 : ' ');
         }

        for (int i = 0; i <= ds_operand_pad; i++) {
            ds_operand[i] = i < ds_operand_len ? toupper(ds_operand[i]) : (i == ds_operand_pad-1 ? 0 : ' ');
        }

        return ds_size;
    }

    // ================ СНАПШОТЫ И СОХРАНЕНИЯ ==========================

    // Загрузка бинарника
    void loadbin(const char* filename, int address)
    {
        FILE* fp = fopen(filename, "rb");
        if (fp == NULL) { printf("BINARY %s not exists\n", filename); exit(1); }
        fseek(fp, 0, SEEK_END);
        int fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fread(ram + address, 1, fsize, fp);
        fclose(fp);
    }

    // Загрузка кастомного ROM в память
    void loadrom(const char* filename, int bank)
    {
        if (bank >= 4) return;

        FILE* fp = fopen(filename, "rb");
        if (fp == NULL) { printf("ROM %s not exists\n", filename); exit(1); }

        // Затирать весь ROM старый, если он там был
        int off = 16384*bank;
        for (int i = 0; i < 16384; i++) rom[off + i] = 0;
        fread(rom + off, 1, 16384, fp);
        fclose(fp);
    }

    // Загрузка скриншота .scr
    void loadScreen(const char* screenshot)
    {
        FILE* fp = fopen(screenshot, "rb");
        if (fp) {

            fread(ram + 0x4000*(port_7ffd & 0x08 ? 7 : 5), 1, 6144+768, fp);
            fclose(fp);

        } else {

            printf("Screenshot not found\n");
            exit(1);
        }
    }
};
