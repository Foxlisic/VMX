// Цвета Спектрума
static const uint32_t colors[16] =
{
    0x000000, 0x0000c0, 0xc00000, 0xc000c0,
    0x00c000, 0x00c0c0, 0xc0c000, 0xc0c0c0,
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
    int width, height, scale, length, pticks;

    // Сохраненные данные на экране
    uint8_t             screen[320][240];
    uint8_t             port7ffd, border;
    int                 ppu_x, ppu_y;

    // Программные данные
    uint8_t             program[65536];         // 3K ROM's
    uint8_t             ram[131072];            // 128К памяти

public:

    VMX(int argc, char** argv);

    int     main();
    int     destroy();
    void    pset(int x, int y, Uint32 cl);
    void    oneframe();
    int     ppu(int cpu_cycles);
};
