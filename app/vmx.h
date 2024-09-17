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

    // Программные данные
    uint8_t             rom[65536];     // 3K ROM's

public:

    VMX(int argc, char** argv);

    int     main();
    int     destroy();
    void    pset(int x, int y, Uint32 cl);
};
