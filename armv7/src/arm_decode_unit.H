#ifndef ARM_DECODE_UNIT
#define ARM_DECODE_UNIT

#include "ac_decoder_rt.H"
#include "ac_instr.H"
#include "ac_instr_info.H"
#include "ac_arch_dec_if.H"
#include "ac_decoded_cache.H"
#include "string.h"

#include "arm_parms.H"
#include "arm_isa.H"

class arm_decode_unit {
public:
  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long shiftamount:5;
    unsigned long shift:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_DPI1;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long rs:4;
    unsigned long subop2:1;
    unsigned long shift:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_DPI2;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long rotate:4;
    unsigned long imm8:8;
  } T_Type_DPI3;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long imm4:4;
    unsigned long rd:4;
    unsigned long imm12:12;
  } T_Type_DPI4;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long shiftamount:5;
    unsigned long tb:1;
    unsigned long subop1:2;
    unsigned long rm:4;
  } T_Type_DPI5;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:7;
    unsigned long msb:5;
    unsigned long rd:4;
    unsigned long lsb:5;
    unsigned long func1:3;
    unsigned long rn:4;
  } T_Type_BTM1;

  typedef struct {
    unsigned long cond:4;
    unsigned long op1:4;
    unsigned long func1:4;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long rotate:4;
    unsigned long func2:4;
    unsigned long rm:4;
  } T_Type_PCK1;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long op1:4;
    unsigned long widthm1:5;
    unsigned long rd:4;
    unsigned long lsb:5;
    unsigned long op2:3;
    unsigned long rn:4;
  } T_Type_MED1;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long h:1;
    unsigned long offset:24;
  } T_Type_BBL;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long h:1;
    unsigned long offset:24;
  } T_Type_BBLT;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long one1:4;
    unsigned long one2:4;
    unsigned long one3:4;
    unsigned long subop2:1;
    unsigned long func2:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_MBXBLX;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long rs:4;
    unsigned long subop2:1;
    unsigned long func2:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_MULT1;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long rdhi:4;
    unsigned long rdlo:4;
    unsigned long rs:4;
    unsigned long subop2:1;
    unsigned long func2:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_MULT2;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long p:1;
    unsigned long u:1;
    unsigned long b:1;
    unsigned long w:1;
    unsigned long l:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long imm12:12;
  } T_Type_LSI;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long p:1;
    unsigned long u:1;
    unsigned long b:1;
    unsigned long w:1;
    unsigned long l:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long shiftamount:5;
    unsigned long shift:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_LSR;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long p:1;
    unsigned long u:1;
    unsigned long i:1;
    unsigned long w:1;
    unsigned long l:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long addr1:4;
    unsigned long subop2:1;
    unsigned long ss:1;
    unsigned long hh:1;
    unsigned long subop1:1;
    unsigned long addr2:4;
  } T_Type_LSE;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long p:1;
    unsigned long u:1;
    unsigned long r:1;
    unsigned long w:1;
    unsigned long l:1;
    unsigned long rn:4;
    unsigned long rlist:16;
  } T_Type_LSM;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:7;
    unsigned long s:1;
    unsigned long rn:4;
    unsigned long rd:4;
    unsigned long subop1:4;
    unsigned long func1:4;
    unsigned long rt:4;
  } T_Type_MEMEX;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long subop3:1;
    unsigned long funcc1:4;
    unsigned long crn:4;
    unsigned long crd:4;
    unsigned long cp_num:4;
    unsigned long funcc3:3;
    unsigned long subop1:1;
    unsigned long crm:4;
  } T_Type_CDP;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long subop3:1;
    unsigned long funcc2:3;
    unsigned long l:1;
    unsigned long crn:4;
    unsigned long rd:4;
    unsigned long cp_num:4;
    unsigned long funcc3:3;
    unsigned long subop1:1;
    unsigned long crm:4;
  } T_Type_CRT;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long p:1;
    unsigned long u:1;
    unsigned long n:1;
    unsigned long w:1;
    unsigned long l:1;
    unsigned long rn:4;
    unsigned long crd:4;
    unsigned long cp_num:4;
    unsigned long imm8:8;
  } T_Type_CLS;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long immediate:12;
    unsigned long subop2:1;
    unsigned long func2:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_MBKPT;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long subop3:1;
    unsigned long swinumber:24;
  } T_Type_MSWI;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:7;
    unsigned long subop1:1;
    unsigned long func1:12;
    unsigned long func2:4;
    unsigned long imm4:4;
  } T_Type_MSMC;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func1:4;
    unsigned long s:1;
    unsigned long one1:4;
    unsigned long rd:4;
    unsigned long one3:4;
    unsigned long subop2:1;
    unsigned long func2:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_MCLZ;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func11:2;
    unsigned long r:1;
    unsigned long func12:2;
    unsigned long fieldmask:4;
    unsigned long rd:4;
    unsigned long zero3:4;
    unsigned long subop2:1;
    unsigned long func2:2;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_MMSR1;

  typedef struct {
    unsigned long cond:4;
    unsigned long op:3;
    unsigned long func11:2;
    unsigned long r:1;
    unsigned long func12:2;
    unsigned long fieldmask:4;
    unsigned long one2:4;
    unsigned long rotate:4;
    unsigned long imm8:8;
  } T_Type_MMSR2;

  typedef struct {
    unsigned long cond:4;
    unsigned long sm:8;
    unsigned long drd:4;
    unsigned long drn:4;
    unsigned long rs:4;
    unsigned long subop2:1;
    unsigned long yy:1;
    unsigned long xx:1;
    unsigned long subop1:1;
    unsigned long rm:4;
  } T_Type_DSPSM;

  typedef struct {
    unsigned id;
    union {
      T_Type_DPI1 F_Type_DPI1;
      T_Type_DPI2 F_Type_DPI2;
      T_Type_DPI3 F_Type_DPI3;
      T_Type_DPI4 F_Type_DPI4;
      T_Type_DPI5 F_Type_DPI5;
      T_Type_BTM1 F_Type_BTM1;
      T_Type_PCK1 F_Type_PCK1;
      T_Type_MED1 F_Type_MED1;
      T_Type_BBL F_Type_BBL;
      T_Type_BBLT F_Type_BBLT;
      T_Type_MBXBLX F_Type_MBXBLX;
      T_Type_MULT1 F_Type_MULT1;
      T_Type_MULT2 F_Type_MULT2;
      T_Type_LSI F_Type_LSI;
      T_Type_LSR F_Type_LSR;
      T_Type_LSE F_Type_LSE;
      T_Type_LSM F_Type_LSM;
      T_Type_MEMEX F_Type_MEMEX;
      T_Type_CDP F_Type_CDP;
      T_Type_CRT F_Type_CRT;
      T_Type_CLS F_Type_CLS;
      T_Type_MBKPT F_Type_MBKPT;
      T_Type_MSWI F_Type_MSWI;
      T_Type_MSMC F_Type_MSMC;
      T_Type_MCLZ F_Type_MCLZ;
      T_Type_MMSR1 F_Type_MMSR1;
      T_Type_MMSR2 F_Type_MMSR2;
      T_Type_DSPSM F_Type_DSPSM;
    };
  } instr_dec;

private:
  typedef ac_decoded_cache<arm_parms::ac_word, instr_dec, 21> ac_decoded_cache_t;

  //!Reference to architecture model. Required to build decoder and
  //!fetch data from memory.
  arm_arch *arch;

  //!Reference to architecture ISA. Required to perform instruction
  //!decoding.
  arm_parms::arm_isa *ISA;

  //!Reference to decoder.
  ac_decoder_full *decoder;

  //!Reference to decoder cache.
  ac_decoded_cache_t *dec_cache;

public:
  arm_decode_unit (arm_arch *arch, arm_parms::arm_isa *ISA);
  instr_dec *decode (arm_parms::ac_word decode_pc);
};
#endif // ARM_DECODE_UNIT