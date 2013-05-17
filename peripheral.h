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

    virtual unsigned read_signal(unsigned address, unsigned offset) = 0;
    virtual void write_signal(unsigned address,
                              unsigned datum, unsigned offset) = 0 ;
};


#endif /* PERIPHERAL_H.  */
