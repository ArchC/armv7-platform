#include<mmu.h>

extern bool DEBUG_MMU;

#define dprintf(args...) if(DEBUG_MMU){fprintf(stderr,args);}

bool MMU::isActive()
{
    //MMU is active if bit 0 of SCC[ControlRegister] is active
    return (cop->SCC_RB[CTR].value & 0x1);
}

ac_tlm_rsp MMU::transport(const ac_tlm_req& req)
{
    dprintf("|| MMU request: <> MMU is: ");

    if(!isActive()){
        dprintf("OFF (bypassing)\n");
        return mainBus->transport(req);
    }

    uint32_t addr = req.addr;

    ///Remap addr
    return mainBus->transport(req);
}
