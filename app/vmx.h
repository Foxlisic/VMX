
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

    // Процессоры (для мультипроцессорной системы)
    int                 cpu_current = 0;
    uint8_t             compat = 1;
    CPU                 cpu[4];
    CPU*                C;
    uint8_t             prefix;

public:

    VMX(int argc, char** argv);

    int         main();
    int         destroy();
    void        pset(int x, int y, Uint32 cl);
    void        oneframe();
    int         ppu(int cpu_cycles);
    void        loadScreen(const char* screenshot);

    // Память
    uint8_t     read(uint16_t a);
    void        write(uint16_t a, uint8_t b);

    // Процессор
    int         step(int core_id);
    void        cpu_put16(int reg_id, uint16_t w);
    uint16_t    cpu_get16(int reg_id);
    int         cpu_condition(int cond);
    void        cpu_update53(uint8_t data);
    void        cpu_setsf(uint8_t a);
    void        cpu_setzf(uint8_t a);
    void        cpu_setof(uint8_t a);
    void        cpu_setpf(uint8_t a);
    void        cpu_setnf(uint8_t a);
    void        cpu_setcf(uint8_t a);
    void        cpu_sethf(uint8_t a);
    uint8_t     cpu_get8(int reg_id);
    void        cpu_put8(int reg_id, uint8_t d);
    uint8_t     cpu_alu(int mode, uint8_t a, uint8_t b);
};
