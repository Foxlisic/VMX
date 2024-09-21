#include <SDL2/SDL.h>

// Цвета Спектрума
static const uint32_t colors[16] =
{
    0x000000, 0x0000BB, 0xBB0000, 0xBB00BB,
    0x00BB00, 0x00BBBB, 0xBBBB00, 0xBBBBBB,
    0x000000, 0x0000FF, 0xFF0000, 0xFF00FF,
    0x00FF00, 0x00FFFF, 0xFFFF00, 0xFFFFFF
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

    // Disassembly.2000
    int     ds_ad;             // Текущая временная позиция разбора инструкции
    int     ds_size;           // Размер инструкции
    char    ds_opcode[16];
    char    ds_operand[32];
    char    ds_prefix[16];
    int     ds_string[64];     // Адреса в строках

    Vz80* core;

public:

    VMX(int argc, char** argv)
    {
        scale       = 2;               // Удвоение пикселей
        width       = 320;             // Ширина экрана
        height      = 240;             // Высота экрана
        length      = (1000/50);       // 50 FPS
        pticks      = 0;
        port_7ffd   = 0x10;

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            exit(1);
        }

        SDL_ClearError();
        sdl_window          = SDL_CreateWindow("ZXSpectrum Verilated Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width, scale * height, SDL_WINDOW_SHOWN);
        sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
        screen_buffer       = (Uint32*) malloc(width * height * sizeof(Uint32));
        sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

        // Загрузка ROM
        for (int i = 0; i < 16384; i++) {

            rom[0x0000 + i] = rom128k[i];
            rom[0x4000 + i] = rom48k[i];
            rom[0x8000 + i] = trdosrom[i];
        }

        // Заполнение таблицы адресов
        for (int y = 0; y < 192; y++) {
            lutfb[y] = 32*((y & 0x38)>>3) + 256*(y&7) + 2048*(y>>6);
        }

        // Перезапуск процессора
        core = new Vz80;
        core->reset_n   = 0;
        core->hold      = 1;
        core->compat    = 1;
        core->irq       = 0;
        core->clock     = 0; core->eval();
        core->clock     = 1; core->eval();
        core->reset_n   = 1;

        loadScreen("../app/scr/aloha.scr");
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
            Uint32 ticks = SDL_GetTicks();

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

            ppu_x = ppu_y = 0;
            Uint32 start = SDL_GetTicks();
            uint8_t brk = 0;

            // 69888=Sinclair Standart Cycles
            // 71680=Pentagonum
            // Длина строки у каждого равна 224
            int cycles_max = 71680;

            // Один кадр
            do {

                for (int i = 0; i < 1024; i++) {

                    // Чтение или запись в память
                    if (core->we)     write(core->address, core->o_data);
                    if (core->portwe) io_write(core->address, core->o_data);

                    core->i_data = read(core->address);
                    core->portin = io_read(core->address);

                    if (core->m0) {

                        disasm(core->address);
                        printf("[%05d] #%04X %s %s\n", cycles, core->address, ds_opcode, ds_operand);
                    }

                    core->clock = 0; core->eval();
                    core->clock = 1; core->eval();
                    cycles++;

                    // В режиме совместимости подсчет циклов
                    if (core->compat) {

                        ppu_clock();
                        ppu_clock();

                        // Запрос #IRQ
                        core->irq = (cycles >= 224*192 && cycles < (224+1)*192);

                        if (cycles >= cycles_max) { brk = 1; break; }
                    }
                }

            } while (SDL_GetTicks() - start < length && (brk == 0));

            // Отрендерить один фрейм в режиме несовместимости
            if (core->compat == 0) for (int i = 0; i < cycles_max; i++) ppu_clock();

            // Мерцающие элементы
            flash_state = (flash_state + 1) % 50;

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

    // Установка точки
    void pset(int x, int y, Uint32 cl)
    {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }

        screen_buffer[width*y + x] = cl;
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
                        pset(cx + j, cy, colors[clr]);
                    }
                }

            } else {

                screen[x][y] = border & 7;
                pset(cx, cy, colors[border & 7]);
            }
        }

        // Размер кадра равен [448 x 312]
        ppu_y = (ppu_y + (ppu_x == 447));
        ppu_x = (ppu_x + 1) % 448;
    }

    // 0x0000-0x3fff ROM
    // 0x4000-0x7fff BANK 2
    // 0x8000-0xbfff BANK 5
    // 0xc000-0xffff BANK 0..7
    int get_bank(int address) {

        int bank = 0;
        switch (address & 0xC000) {

            case 0x0000: bank = (port_7ffd & 0x30) ? 1 : 0; break;
            case 0x4000: bank = 5; break;
            case 0x8000: bank = 2; break;
            case 0xC000: bank = (port_7ffd & 7); break;
        }

        return bank*16384 + (address & 0x3FFF);
    }

    // Чтение байта
    int read(int address) {

        // Обращение к ROM 128k|48k (0 или 16384)
        if (address < 0x4000) {
            return trdos_latch ? rom[0x8000 + address] : rom[get_bank(address)];
        }

        return ram[get_bank(address)];
    }

    // Запись байта
    void write(int address, int data) {

        address &= 0xFFFF;
        if (address < 0x4000) return;

        ram[get_bank(address)] = data;
    }

    // Чтение из порта
    int io_read(int port) {

        // Порт #7FFD
        if (port == 0x7FFD) {
            return port_7ffd;
        }
        // else if (port == 0xFFFD) { return ay_register; }
        // else if (port == 0xBFFD) { return ay_regs[ay_register%15]; }
        // Чтение клавиатуры
        else if ((port & 1) == 0) {

            // Чтение из порта во время движения луча по бордеру
            // if (contended_mem && beam_drawing && !beam_in_paper) { cycle_counter++; }

            int result = 0xff;
            for (int row = 0; row < 8; row++) {
                if (!(port & (1 << (row + 8)))) {
                    result &= key_states[ row ];
                }
            }

            return result;
        }
        // Kempston Joystick
        else if ((port & 0x00e0) == 0x0000) {
            return 0x00;
        }

        return 0xff;
    }

    // Запись в порт
    void io_write(int port, int data) {

        if (port == 0x7ffd) {

            // Не менять бит D5 если он 1
            if ((port_7ffd & 0x20) && (data & 0x20) == 0)
                data |= 0x20;

            port_7ffd = data;
        }
        // AY address register
        // else if (port == 0xFFFD) { ay_register = data & 15; }
        // AY address data
        // else if (port == 0xBFFD) { ay_write_data(data); }
        else if (port == 0x1FFD) { /* ничего пока что */ }
        else if ((port & 1) == 0) {

            // Чтение в порт во время движения луча по бордеру
            // if (contended_mem && beam_drawing && !beam_in_paper) { cycle_counter++; }

            border  = (data & 7);
            port_fe = data;
        }
    }

    // Проверяется наличие входа и выхода из TRDOS
    void trdos_handler(uint16_t pc) {

        // Only 48k ROM allowed
        if (port_7ffd & 0x10) {

            // Вход в TRDOS : инструкция находится в адресе 3Dh
            if      (!trdos_latch && (pc & 0xFF00) == 0x3D00) { trdos_latch = 1; }
            // Выход из TRDOS
            else if ( trdos_latch && (pc & 0xC000))           { trdos_latch = 0; }
        }
    }

    // Занесение нажатия в регистры
    void key_press(int row, int mask, int press) {

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

            // F1-F12 Клавиши
            /*
            case SDL_SCANCODE_F1:   if (release) kbd_push(0xF0); kbd_push(0x05); break;
            case SDL_SCANCODE_F2:   if (release) kbd_push(0xF0); kbd_push(0x06); break;
            case SDL_SCANCODE_F3:   if (release) kbd_push(0xF0); kbd_push(0x04); break;
            case SDL_SCANCODE_F4:   if (release) kbd_push(0xF0); kbd_push(0x0C); break;
            case SDL_SCANCODE_F5:   if (release) kbd_push(0xF0); kbd_push(0x03); break;
            case SDL_SCANCODE_F6:   if (release) kbd_push(0xF0); kbd_push(0x0B); break;
            case SDL_SCANCODE_F7:   if (release) kbd_push(0xF0); kbd_push(0x83); break;
            case SDL_SCANCODE_F8:   if (release) kbd_push(0xF0); kbd_push(0x0A); break;
            case SDL_SCANCODE_F9:   if (release) kbd_push(0xF0); kbd_push(0x01); break;
            case SDL_SCANCODE_F10:  if (release) kbd_push(0xF0); kbd_push(0x09); break;
            case SDL_SCANCODE_F11:  if (release) kbd_push(0xF0); kbd_push(0x78); break;
            case SDL_SCANCODE_F12:  if (release) kbd_push(0xF0); kbd_push(0x07); break;

            // Расширенные клавиши
            case SDL_SCANCODE_LGUI:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x1F); break;
            case SDL_SCANCODE_RGUI:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x27); break;
            case SDL_SCANCODE_APPLICATION:  kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x2F); break;
            case SDL_SCANCODE_RCTRL:        kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x14); break;
            case SDL_SCANCODE_RALT:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x11); break;

            case SDL_SCANCODE_INSERT:       kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x70); break;
            case SDL_SCANCODE_HOME:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x6C); break;
            case SDL_SCANCODE_END:          kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x69); break;
            case SDL_SCANCODE_PAGEUP:       kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x7D); break;
            case SDL_SCANCODE_PAGEDOWN:     kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x7A); break;
            case SDL_SCANCODE_DELETE:       kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x71); break;
            */

            case SDL_SCANCODE_UP:           key_press(0, 0x01, press); key_press(4, 0x08, press); break; // SS+7
            case SDL_SCANCODE_DOWN:         key_press(0, 0x01, press); key_press(4, 0x10, press); break; // SS+6
            case SDL_SCANCODE_LEFT:         key_press(0, 0x01, press); key_press(3, 0x10, press); break; // SS+5
            case SDL_SCANCODE_RIGHT:        key_press(0, 0x01, press); key_press(4, 0x04, press); break; // SS+8

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

    // ====================== DISASSEMBY 2000 ==========================

    // Сформировать операнд (IX|IY+d)
    void ixy_disp(int prefix) {

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
    int ds_fetch_byte() {

        int b = read(ds_ad);
        ds_ad = (ds_ad + 1) & 0xffff;
        ds_size++;
        return b;
    }

    // Прочитать слово дизассемблера
    int ds_fetch_word() {

        int l = ds_fetch_byte();
        int h = ds_fetch_byte();
        return (h<<8) | l;
    }

    // Прочитать относительный операнд
    int ds_fetch_rel() {

        int r8 = ds_fetch_byte();
        return ((r8 & 0x80) ? r8 - 0x100 : r8) + ds_ad;
    }

    // Дизассемблирование 1 линии
    int disasm(int addr) {

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

        int ds_opcode_len = strlen(ds_opcode);
        int ds_opcode_pad = ds_opcode_len < 5 ? 5 : ds_opcode_len + 1;

        // Привести в порядок
        for (int i = 0; i <= ds_opcode_pad; i++) {
             ds_opcode[i] = i < ds_opcode_len ? toupper(ds_opcode[i]) : (i == ds_opcode_pad-1 ? 0 : ' ');
         }

        for (int i = 0; i < strlen(ds_operand); i++) ds_operand[i] = toupper(ds_operand[i]);

        return ds_size;
    }
};
