#include <SDL2/SDL.h>

#include "roms/128k.h"
#include "roms/48k.h"
#include "roms/trdos.h"

#include "vmx.h"
#include "vmx.cpp"

int main(int argc, char* argv[])
{
    VMX* vmx = new VMX(argc, argv);
    while (vmx->main());
    return 0;
}
