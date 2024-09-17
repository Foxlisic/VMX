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
    int                 width, height, scale, length, pticks;

    // Сохраненные данные на экране
    uint8_t             screen[352][304];
    uint8_t             port_7ffd, border;
    int                 lutfb[192];
    int                 ppu_x, ppu_y, contended_mem, flash_state = 0;

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
    void    loadScreen(const char* screenshot);
};
