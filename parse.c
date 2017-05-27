#include <stdio.h>
#include <runt.h>
#include "img.h"

static runt_int loader(runt_vm *vm)
{
    runt_load_stdlib(vm);
    runt_load_img(vm);
    return RUNT_OK;
}

int main(int argc, char *argv[])
{
    return irunt_begin(argc, argv, loader);
}
