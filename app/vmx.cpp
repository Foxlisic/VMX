// Запуск окна, инициализация, загрузка аргументов
VMX::VMX(int argc, char** argv)
{
    scale       = 2;                // Удвоение пикселей
    width       = 320;              // Ширина экрана [48+256+48]
    height      = 240;              // Высота экрана [56+192+56]
    length      = (1000/50);        // 50 кадров в секунду
    pticks      = 0;                // Отсчет кадра

    border      = 0;                // Цвет бордера
    port_7ffd   = 0x10;             // 48К по умолчанию
    ppu_x       = 0;
    ppu_y       = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        exit(1);
    }

    // Активация окна
    SDL_ClearError();
    sdl_window          = SDL_CreateWindow("VMX: Spectrum Emulgator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale*width, scale*height, SDL_WINDOW_SHOWN);
    sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
    screen_buffer       = (Uint32*) malloc(width * height * sizeof(Uint32));
    sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

    // Загрузка ROM
    for (int i = 0; i < 16384; i++) {

        program[0x0000 + i] = rom128k[i];
        program[0x4000 + i] = rom48k[i];
        program[0x8000 + i] = trdosrom[i];
    }

    // Заполнение таблицы адресов
    for (int y = 0; y < 192; y++) {
        lutfb[y] = 32*((y & 0x38)>>3) + 256*(y&7) + 2048*(y>>6);
    }

    loadScreen("scr/firecast.scr");
}

// Загрузка скриншота .scr
void VMX::loadScreen(const char* screenshot)
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

// Главный цикл работы
int VMX::main()
{
    SDL_Rect dstRect;

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = scale * width;
    dstRect.h = scale * height;

    for (;;) {

        Uint32 ticks = SDL_GetTicks();

        // Прием событий
        while (SDL_PollEvent(& evt)) {

            // Событие выхода
            switch (evt.type) {
                case SDL_QUIT: return 0;
            }
        }

        // Обновление экрана
        if (ticks - pticks >= length) {

            pticks = ticks;

            oneframe();

            SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, width*sizeof(Uint32));
            SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
            SDL_RenderClear         (sdl_renderer);
            SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, &dstRect);
            SDL_RenderPresent       (sdl_renderer);

            return 1;
        }

        SDL_Delay(1);
    }
}

// Убрать окно из памяти
int VMX::destroy()
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
void VMX::pset(int x, int y, Uint32 cl)
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    screen_buffer[width*y + x] = cl;
}

// Отсчет одного фрейма
void VMX::oneframe()
{
    int all_cycles = 0;

    ppu_x = 0;
    ppu_y = 0;

    while (all_cycles < 448*312) {

        // Симуляция такта
        int cycles = 1;
        all_cycles += ppu(cycles);
    }

    // Мерцающие элементы
    flash_state = (flash_state + 1) % 50;
}

// Отсчитать количество тактов PPU
int VMX::ppu(int cpu_cycles)
{
    cpu_cycles *= 2;

    for (int i = 0; i < cpu_cycles; i++) {

        // Реальные размеры
        int x = ppu_x - 96,
            y = ppu_y - 8;

        // Коррекция под экран 320x200
        int cx = x - 16,
            cy = y - 32;

        // Указывает, что при записи или чтении видеопамяти будет добавляться +1 такт CPU
        contended_mem = 0;

        // Видимая область
        if (x >= 0 && y >= 0) {

            // Область рисования PAPER
            if (x >= 48 && x < 48+256 && y >= 56 && y < 56+192) {

                contended_mem = (port_7ffd & 0x30) ? 1 : 0;

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
        ppu_y = (ppu_y + (ppu_x == 447)) % 312;
        ppu_x = (ppu_x + 1) % 448;
    }

    return cpu_cycles;
}

// Чтение адреса из памяти, в зависимости от банков
uint8_t VMX::read(uint16_t a)
{
    return 0;
}

// Запись в память
void VMX::write(uint16_t a, uint8_t b)
{
}
