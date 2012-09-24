/**
 * This module implements a base interface for any peripheral connected
 * to the Core model.
 * Author: Gabriel Krisman Bertazi
 **/

#ifndef  __PERIPHERAL_H__
#define  __PERIPHERAL_H__
#include<stdint.h>

class peripheral {

protected:
    uint32_t mem_range_start;
    uint32_t mem_range_end;

public:
    unsigned GetMemoryBegin()
    {
        return mem_range_start;
    }
    unsigned GetMemoryEnd()
    {
        return mem_range_end;
    }

    virtual unsigned read_signal(unsigned address, unsigned offset) = 0;
    virtual void write_signal(unsigned address,
                              unsigned datum, unsigned offset) = 0 ;
   

peripheral (uint32_t start_add, uint32_t end_add)
    : mem_range_start(start_add), mem_range_end(end_add) { }
};


#endif
