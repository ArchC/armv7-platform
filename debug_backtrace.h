#ifndef __DEBUG_BACKTRACE_H__
#define __DEBUG_BACKTRACE_H__

#include <stdint.h> // define types uint32_t, etc
extern bool DEBUG_FLOW;
static int debug_btlevel = 0;

#define dprintbt_enter(entry, ret) if(DEBUG_FLOW) dprint_beginFunc(entry, ret);
#define dprintbt_leave()           if(DEBUG_FLOW) dprint_endFunc();
#define dprintbt_verifyEnd(lr)     dprint_verifyEndOfFunc(lr)

void dprint_beginFunc(uint32_t entry_add, uint32_t return_add);
void dprint_endFunc();
bool dprint_verifyEndOfFunc(uint32_t lr);
void dprint_flow_indentation();


#endif
