#include <verilated.h>
#include "obj_dir/Vz80.h"

#include <128k.h>
#include <48k.h>
#include "vmx.h"

int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);
    VMX* vmx = new VMX(argc, argv);
    while (vmx->main());
    return vmx->destroy();
}
