/**
 * @file      arm_gdb_funcs.cpp
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

#include "arm.H"

using namespace arm_parms;

unsigned readCPSR();
void writeCPSR(unsigned);
unsigned bypass_read(unsigned address);
void bypass_write(unsigned address, unsigned datum);

int arm::nRegs(void) {
   return 17;
}

ac_word arm::reg_read( int reg ) {
  
  /* general purpose registers */
  if ( ( reg >= 0 ) && ( reg < 15 ) )
    return bypass_read( reg );
  else if ( reg == 15 )
    return ac_pc;
  else /* cpsr */
    return readCPSR();
  return 0;
}

void arm::reg_write( int reg, ac_word value ) {
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

unsigned char arm::mem_read( unsigned int address ) {
  unsigned offset = (address % 4) * 8;
  unsigned res;

  address = (address >> 2) << 2;

  res =  MEM.read(address);
  if (offset)
    res = res >> offset;
  return (unsigned char) res & 0xff;

}

void arm::mem_write( unsigned int address, unsigned char byte ) {
  MEM.write_byte( address, byte );
}

