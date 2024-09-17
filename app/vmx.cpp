// Запуск окна, инициализация, загрузка аргументов
VMX::VMX(int argc, char** argv)
{
    scale       = 2;                // Удвоение пикселей
    width       = 320;              // Ширина экрана
    height      = 240;              // Высота экрана
    length      = (1000/50);        // 50 кадров в секунду
    pticks      = 0;                // Отсчет кадра

    border      = 0;                // Цвет бордера
    port7ffd    = 0x10;             // 48К по умолчанию
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
    while (all_cycles < 448*312) {

        int cycles = 1; // Симуляция такта
        all_cycles += ppu(cycles);
    }
}

// Отсчитать количество тактов PPU
int VMX::ppu(int cpu_cycles)
{
    cpu_cycles *= 2;

    for (int i = 0; i < cpu_cycles; i++) {

        // Размер кадра равен [448 x 312]
        ppu_y = (ppu_y + (ppu_x == 447)) % 312;
        ppu_x = (ppu_x + 1) % 448;
    }

    return cpu_cycles;
}
