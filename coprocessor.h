#ifndef __COPROCESSOR_H__
#define __COPROCESSOR_H__

class coprocessor
{
    virtual void reset()=0;
public:
    virtual void MCR(unsigned opc1, unsigned opc2, unsigned crn,
                     unsigned crm, unsigned rt_value)=0;

    

};

#endif
