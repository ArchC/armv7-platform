/******************************************************
 * ArchC Resources implementation file.               *
 * This file is automatically generated by ArchC      *
 * WITHOUT WARRANTY OF ANY KIND, either express       *
 * or implied.                                        *
 * For more information on ArchC, please visit:       *
 * http://www.archc.org                               *
 *                                                    *
 * The ArchC Team                                     *
 * Computer Systems Laboratory (LSC)                  *
 * IC-UNICAMP                                         *
 * http://www.lsc.ic.unicamp.br                       *
 ******************************************************/
 

#include  "armv5e_arch.H"
#include  "armv5e_arch_ref.H"

//!/Default constructor.
armv5e_arch_ref::armv5e_arch_ref(armv5e_arch& arch) : ac_arch_ref<armv5e_parms::ac_word, armv5e_parms::ac_Hword>(arch),
  ac_pc(arch.ac_pc),
  MEM(arch.MEM),   RB(arch.RB),   R14_irq(arch.R14_irq),   R14_fiq(arch.R14_fiq),   R14_svc(arch.R14_svc),   R14_abt(arch.R14_abt),   R14_und(arch.R14_und),   R13_irq(arch.R13_irq),   R13_svc(arch.R13_svc),   R13_abt(arch.R13_abt),   R13_und(arch.R13_und),   R13_fiq(arch.R13_fiq),   SPSR_irq(arch.SPSR_irq),   SPSR_fiq(arch.SPSR_fiq),   SPSR_svc(arch.SPSR_svc),   SPSR_abt(arch.SPSR_abt),   SPSR_und(arch.SPSR_und),   R12_fiq(arch.R12_fiq),   R11_fiq(arch.R11_fiq),   R10_fiq(arch.R10_fiq),   R9_fiq(arch.R9_fiq),   R8_fiq(arch.R8_fiq) {}

