/**
 * This module implements a base interface for any peripheral connected
 * to the Core model.
 * Author: Gabriel Krisman Bertazi
 **/

#ifndef  PERIPHERAL_H
#define  PERIPHERAL_H

#include<stdint.h>

class peripheral {

public:

    virtual uint32_t read_signal(uint32_t address, uint32_t offset) = 0;
    virtual void write_signal(uint32_t address, uint32_t datum,
                              uint32_t offset) = 0 ;
};


#endif /* PERIPHERAL_H.  */
