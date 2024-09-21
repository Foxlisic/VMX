#include <SDL2/SDL.h>

// Цвета Спектрума
static const uint32_t colors[16] =
{
    0x000000, 0x0000BB, 0xBB0000, 0xBB00BB,
    0x00BB00, 0x00BBBB, 0xBBBB00, 0xBBBBBB,
    0x000000, 0x0000FF, 0xFF0000, 0xFF00FF,
    0x00FF00, 0x00FFFF, 0xFFFF00, 0xFFFFFF
};

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

    uint8_t     screen[352][304];
    uint8_t     port_7ffd, border, flash_state = 0;

    Vz80* core;

public:

    VMX(int argc, char** argv)
    {
        scale   = 2;               // Удвоение пикселей
        width   = 320;             // Ширина экрана
        height  = 240;             // Высота экрана
        length  = (1000/50);       // 50 FPS
        pticks  = 0;
        port_7ffd = 0x10;

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            exit(1);
        }

        SDL_ClearError();
        sdl_window          = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width, scale * height, SDL_WINDOW_SHOWN);
        sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
        screen_buffer       = (Uint32*) malloc(width * height * sizeof(Uint32));
        sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

        // Загрузка ROM
        for (int i = 0; i < 16384; i++) {

            ram[0x0000 + i] = rom128k[i];
            ram[0x4000 + i] = rom48k[i];
            ram[0x8000 + i] = trdosrom[i];
        }

        // Заполнение таблицы адресов
        for (int y = 0; y < 192; y++) {
            lutfb[y] = 32*((y & 0x38)>>3) + 256*(y&7) + 2048*(y>>6);
        }

        // Перезапуск процессора
        core = new Vz80;
        core->reset_n   = 0;
        core->hold      = 1;
        core->compat    = 0;
        core->irq       = 0;
        core->clock     = 0; core->eval();
        core->clock     = 1; core->eval();
        core->reset_n   = 0;

        loadScreen("../app/scr/fox.scr");
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
                switch (evt.type) { case SDL_QUIT: return 0; }
            }

            Uint32 start = SDL_GetTicks();

            // Один кадр
            do {

                for (int i = 0; i < 1024; i++) {

                    core->clock = 0; core->eval();
                    core->clock = 1; core->eval();
                    cycles++;
                }

            } while (SDL_GetTicks() - start < length);

            // Отрендерить один фрейм
            ppu_x = ppu_y = 0; for (int i = 0; i < 448*312; i++) ppu_clock();

            // Мерцающие элементы
            flash_state = (flash_state + 1) % 50;

            // Обновление экрана
            SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, width * sizeof(Uint32));
            SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
            SDL_RenderClear         (sdl_renderer);
            SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, & dstRect);
            SDL_RenderPresent       (sdl_renderer);

            SDL_Delay(1);
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
        ppu_y = (ppu_y + (ppu_x == 447)) % 312;
        ppu_x = (ppu_x + 1) % 448;
    }
};
