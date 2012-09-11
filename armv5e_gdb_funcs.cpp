/**
 * @file      armv5e_gdb_funcs.cpp
 * @author    Danilo Marcolin Caravana
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:33:28 -0300
 * 
 * @brief     The ArchC ARMv5e functional model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 *
 */

#include "armv5e.H"

using namespace armv5e_parms;

unsigned readCPSR();
void writeCPSR(unsigned);
unsigned bypass_read(unsigned address);
void bypass_write(unsigned address, unsigned datum);

int armv5e::nRegs(void) {
   return 17;
}

ac_word armv5e::reg_read( int reg ) {
  
  /* general purpose registers */
  if ( ( reg >= 0 ) && ( reg < 15 ) )
    return bypass_read( reg );
  else if ( reg == 15 )
    return ac_pc;
  else /* cpsr */
    return readCPSR();
  return 0;
}

void armv5e::reg_write( int reg, ac_word value ) {
  /* general purpose registers */
  printf("Register is: %d, value is %x\n",reg,value);
  if ( ( reg >= 0 ) && ( reg < 15 ) )
    bypass_write( reg, value );
  else if ( reg == 15 )
    /* pc */
    ac_pc = value;
  else /* CPSR */
    writeCPSR (value);
}

unsigned char armv5e::mem_read( unsigned int address ) {
  return MEM.read_byte(address);
}

void armv5e::mem_write( unsigned int address, unsigned char byte ) {
  MEM.write_byte( address, byte );
}

