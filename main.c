#include "log.h"
#include "common.h"
#include "spu.h"


int main() {
    logInit("log.txt");
    printfOK("start program...\n");
 
    spu_t spu = {};
    ErrorStatusSpu err_ctor = spuCtor(&spu, "asm.bin");
    if (err_ctor != SPU_OK) {
        printfError("no memory ctor\n");
        return 1;
    }
    
    ErrorStatusSpu err_run = spuRun(&spu);
    if (err_run != SPU_OK) {
        printfError("bad run\n");
        return 1;
    }
    printfOK("all ok\n");
    
    spuDtor(&spu);
    logDestroy();
    return 0;
}

