/* dma_device.h
 *
 * This file defines an interface for peripherals
 * who intend to use DMA services.
 */
#ifndef  __DMA_DEVICE_H__
#define  __DMA_DEVICE_H__

#include <stdint.h> // define types uint32_t, etc
class dma_device
{

protected:
    uint32_t mem_range_start;
    uint32_t mem_range_end;

public:
    unsigned dma_address_begin()
    {
        return mem_range_start;
    }
    unsigned dma_address_end()
    {
        return mem_range_end;
    }

dma_device(unsigned dma_address_start_, unsigned dma_address_end_):
    dma_address_start(dma_addess_start_), dma_address_end(dma_address_end_){}

}

#endif
