#ifndef __COPROCESSOR_H__
#define __COPROCESSOR_H__

#include "arm_interrupts.h"
class coprocessor
{
    virtual void reset()=0;
public:
    virtual void MCR(arm_arch_ref *core,
                     arm_impl::PrivilegeLevel pl,
                     unsigned opc1,
                     unsigned opc2,
                     unsigned crn,
                     unsigned crm,
                     unsigned rt_value)=0;

    virtual unsigned MRC(arm_arch_ref *core,
                         arm_impl::PrivilegeLevel pl,
                         unsigned opc1,
                         unsigned opc2,
                         unsigned crn,
                         unsigned crm)=0;

};

#endif
