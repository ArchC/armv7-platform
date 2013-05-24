#include "bus.h"

extern bool DEBUG_BUS;
#define dprintf(args...) if(DEBUG_BUS){fprintf(stderr,args);}

imx53_bus::~imx53_bus() {}

//Include a new device to array.
void imx53_bus::connect_device(peripheral *device,
                               uint32_t start_address, uint32_t end_address)
{
    struct imx53_bus::mapped_device new_connection;
    new_connection.device = device;
    new_connection.start_address = start_address;
    new_connection.end_address = end_address;

    devices.push_back(new_connection);
}


//Route a request to an specific device.
ac_tlm_rsp imx53_bus::transport(const ac_tlm_req& req){

    std::deque<struct mapped_device >::iterator it = devices.begin();
    struct imx53_bus::mapped_device *cur;

    ac_tlm_rsp ans;
    unsigned addr = req.addr;
    unsigned offset = (addr % 4) * 8;
    addr = (addr >> 2) << 2;
    ans.status = SUCCESS;
    if (offset) {
        dprintf(" ! Next bus access is misaligned. Byte offset: %d\n",
                offset/8);
    }

    while(it != devices.end()){
        /* dereference operator is overloaded so we need this trick.  */
        cur = &(*it);
        it++;
        if(addr >= cur->start_address && addr <= cur->end_address){
            if(req.type == READ){
                dprintf(" <--> BUS TRANSACTION: [READ] 0x%X\n", addr);

                ans.data = cur->device->read_signal((addr - cur->start_address), offset);
                if (offset){
                    ans.data = ans.data >> offset;
                }
                return ans;
            }
            else if(req.type == WRITE){
                dprintf(" <--> BUS TRANSACTION: [WRITE] 0x%X @0x%X \n",req.data, addr);

                cur->device->write_signal((addr - cur->start_address),
                                          req.data, offset);
                return ans;
            }
        }
    }

    /* Fail - warn core about failure.  */
    ac_tlm_req abrt_req;
    abrt_req.type = READ;
    abrt_req.dev_id = 0;
    abrt_req.addr = 0;
    abrt_req.data = arm_impl::EXCEPTION_DATA_ABORT;
    proc_port->transport(abrt_req);
    ans.status = ERROR;

return ans;
}
