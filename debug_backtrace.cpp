#include "debug_backtrace.h"
#include<stack>
#include<stdio.h>

static  std::stack<uint32_t> rets;

void dprint_beginFunc(uint32_t entry_add, uint32_t return_add)
{
        debug_btlevel++;
    dprint_flow_indentation();
    fprintf(stderr,"<Entered Func => 0x%X.  Return address => 0x%X>\n", entry_add, return_add);

    rets.push(return_add);
}

void dprint_endFunc()
{

    if(rets.size() == 0) return;
    dprint_flow_indentation();
    fprintf(stderr, "<Returning to address 0x%X>\n", rets.top());
    debug_btlevel--;
    rets.pop();
}
bool dprint_verifyEndOfFunc(uint32_t lr)
{
    if(DEBUG_FLOW && rets.size() > 0)
         return (lr == rets.top());
     else
         return false;
}
void dprint_flow_indentation()
{
    for(int i =0;i< debug_btlevel;i++) fprintf(stderr,"    ");
}
