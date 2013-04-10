#include "bus.h"

extern bool DEBUG_BUS;

static inline int dprintf(const char *format, ...) {
    int ret;
    if (DEBUG_BUS) {
        va_list args;
        va_start(args, format);
        ret = vfprintf(ac_err, format, args);
        va_end(args);
    }
    return ret;
}

imx53_bus::~imx53_bus() {}

//Include a new device to array.
void imx53_bus::connectDevice(peripheral *device){
    devices.push_back(device);
}


//Route a request to an specific device.
ac_tlm_rsp imx53_bus::transport(const ac_tlm_req& req){

    deque<peripheral*>::iterator it = devices.begin();
    peripheral *actual;

    ac_tlm_rsp ans;
    unsigned addr = req.addr;
    unsigned offset = (addr % 4) * 8;
    addr = (addr >> 2) << 2;
    ans.status = SUCCESS;
    if (offset) {
        dprintf(" ! Next bus access is misaligned. Byte offset: %d\n", offset/8);
    }

    while(it != devices.end()){
        actual = *it;
        it++;

        if(addr >= actual->GetMemoryBegin() && addr <= actual->GetMemoryEnd()){
            if(req.type == READ){
                dprintf(" <--> BUS TRANSACTION: [READ] 0x%X\n", addr);
                ans.data = actual->read_signal((addr - actual->GetMemoryBegin()), offset);
                if (offset){
                    ans.data = ans.data >> offset;
                }
                return ans;
            }
            else if(req.type == WRITE){
                dprintf(" <--> BUS TRANSACTION: [WRITE] 0x%X @0x%X \n",req.data, addr);
                actual->write_signal((addr - actual->GetMemoryBegin()) , req.data, offset);
                return ans;
            }
            break;
        }
    }


//     if(req.type == WRITE){
// //        printf(" <--> BUS TRANSACTION: FAILED WRITE address: 0x%X content:0x%X (*NOT MAPPED*)\n", addr,req.data);
//         fflush(stdout);
//         //abort();
//     }
//     else /* READ */
//         //      printf(" <--> BUS TRANSACTION: FAILED READ address: 0x%X (*NOT MAPPED*)\n", addr);

// //    if((req.addr &0xFFFF0000) == 0x63FC0000) abort();

    // Fail - warn core about failure
    ac_tlm_req abrt_req;
    abrt_req.type = READ;
    abrt_req.dev_id = 0;
    abrt_req.addr = 0;
    abrt_req.data = arm_impl::EXCEPTION_DATA_ABORT;
    proc_port->transport(abrt_req);
    ans.status = ERROR;


    return ans;
}
