#include "arm_decode_unit.H"

arm_decode_unit::arm_decode_unit (arm_arch *arch_, arm_parms::arm_isa *ISA_) {

  arch = arch_;
  ISA = ISA_;
  decoder = ac_decoder_full::CreateDecoder (ISA->formats, ISA->instructions, arch);
  dec_cache = new ac_decoded_cache_t ();
}

arm_decode_unit::instr_dec *arm_decode_unit::decode (arm_parms::ac_word decode_pc) {

  unsigned *ins_cache = NULL;
  arm_decode_unit::instr_dec *instr_dec = NULL;
  arm_parms::ac_fetch buffer[4];
  const unsigned quant = 1;

  //!Perfom Instruction Fetch.
  for(int i = 0; i < quant; i++)
    buffer[i] = arch->IM->read (decode_pc + i*sizeof(arm_parms::ac_word));

  //!Attempt to use decoded cache.
  instr_dec = dec_cache->fetch_item (decode_pc, buffer, quant);

  if (instr_dec) {
    return instr_dec;
  }
  //!Perform decode.
  ins_cache = decoder->Decode (reinterpret_cast<unsigned char *>(buffer), quant);

  //!Insert decoded instruction into decoded cache.
  instr_dec = dec_cache->insert_item (decode_pc, buffer, quant);

  instr_dec->id = ins_cache[IDENT];
  switch (ISA->instr_format_table[instr_dec->id]) {
    case 0:
      instr_dec->F_Type_DPI1.cond = ins_cache[1];
      instr_dec->F_Type_DPI1.op = ins_cache[2];
      instr_dec->F_Type_DPI1.func1 = ins_cache[3];
      instr_dec->F_Type_DPI1.s = ins_cache[4];
      instr_dec->F_Type_DPI1.rn = ins_cache[5];
      instr_dec->F_Type_DPI1.rd = ins_cache[6];
      instr_dec->F_Type_DPI1.shiftamount = ins_cache[7];
      instr_dec->F_Type_DPI1.shift = ins_cache[8];
      instr_dec->F_Type_DPI1.subop1 = ins_cache[9];
      instr_dec->F_Type_DPI1.rm = ins_cache[10];
      break;
    case 1:
      instr_dec->F_Type_DPI2.cond = ins_cache[1];
      instr_dec->F_Type_DPI2.op = ins_cache[2];
      instr_dec->F_Type_DPI2.func1 = ins_cache[3];
      instr_dec->F_Type_DPI2.s = ins_cache[4];
      instr_dec->F_Type_DPI2.rn = ins_cache[5];
      instr_dec->F_Type_DPI2.rd = ins_cache[6];
      instr_dec->F_Type_DPI2.rs = ins_cache[11];
      instr_dec->F_Type_DPI2.subop2 = ins_cache[12];
      instr_dec->F_Type_DPI2.shift = ins_cache[8];
      instr_dec->F_Type_DPI2.subop1 = ins_cache[9];
      instr_dec->F_Type_DPI2.rm = ins_cache[10];
      break;
    case 2:
      instr_dec->F_Type_DPI3.cond = ins_cache[1];
      instr_dec->F_Type_DPI3.op = ins_cache[2];
      instr_dec->F_Type_DPI3.func1 = ins_cache[3];
      instr_dec->F_Type_DPI3.s = ins_cache[4];
      instr_dec->F_Type_DPI3.rn = ins_cache[5];
      instr_dec->F_Type_DPI3.rd = ins_cache[6];
      instr_dec->F_Type_DPI3.rotate = ins_cache[11];
      instr_dec->F_Type_DPI3.imm8 = ins_cache[13];
      break;
    case 3:
      instr_dec->F_Type_DPI4.cond = ins_cache[1];
      instr_dec->F_Type_DPI4.op = ins_cache[2];
      instr_dec->F_Type_DPI4.func1 = ins_cache[3];
      instr_dec->F_Type_DPI4.s = ins_cache[4];
      instr_dec->F_Type_DPI4.imm4 = ins_cache[5];
      instr_dec->F_Type_DPI4.rd = ins_cache[6];
      instr_dec->F_Type_DPI4.imm12 = ins_cache[14];
      break;
    case 4:
      instr_dec->F_Type_DPI5.cond = ins_cache[1];
      instr_dec->F_Type_DPI5.op = ins_cache[2];
      instr_dec->F_Type_DPI5.func1 = ins_cache[3];
      instr_dec->F_Type_DPI5.s = ins_cache[4];
      instr_dec->F_Type_DPI5.rn = ins_cache[5];
      instr_dec->F_Type_DPI5.rd = ins_cache[6];
      instr_dec->F_Type_DPI5.shiftamount = ins_cache[7];
      instr_dec->F_Type_DPI5.tb = ins_cache[15];
      instr_dec->F_Type_DPI5.subop1 = ins_cache[16];
      instr_dec->F_Type_DPI5.rm = ins_cache[10];
      break;
    case 5:
      instr_dec->F_Type_BTM1.cond = ins_cache[1];
      instr_dec->F_Type_BTM1.op = ins_cache[17];
      instr_dec->F_Type_BTM1.msb = ins_cache[18];
      instr_dec->F_Type_BTM1.rd = ins_cache[6];
      instr_dec->F_Type_BTM1.lsb = ins_cache[7];
      instr_dec->F_Type_BTM1.func1 = ins_cache[19];
      instr_dec->F_Type_BTM1.rn = ins_cache[10];
      break;
    case 6:
      instr_dec->F_Type_PCK1.cond = ins_cache[1];
      instr_dec->F_Type_PCK1.op1 = ins_cache[20];
      instr_dec->F_Type_PCK1.func1 = ins_cache[21];
      instr_dec->F_Type_PCK1.rn = ins_cache[5];
      instr_dec->F_Type_PCK1.rd = ins_cache[6];
      instr_dec->F_Type_PCK1.rotate = ins_cache[11];
      instr_dec->F_Type_PCK1.func2 = ins_cache[22];
      instr_dec->F_Type_PCK1.rm = ins_cache[10];
      break;
    case 7:
      instr_dec->F_Type_MED1.cond = ins_cache[1];
      instr_dec->F_Type_MED1.op = ins_cache[2];
      instr_dec->F_Type_MED1.op1 = ins_cache[3];
      instr_dec->F_Type_MED1.widthm1 = ins_cache[18];
      instr_dec->F_Type_MED1.rd = ins_cache[6];
      instr_dec->F_Type_MED1.lsb = ins_cache[7];
      instr_dec->F_Type_MED1.op2 = ins_cache[19];
      instr_dec->F_Type_MED1.rn = ins_cache[10];
      break;
    case 8:
      instr_dec->F_Type_BBL.cond = ins_cache[1];
      instr_dec->F_Type_BBL.op = ins_cache[2];
      instr_dec->F_Type_BBL.h = ins_cache[23];
      instr_dec->F_Type_BBL.offset = ins_cache[24];
      break;
    case 9:
      instr_dec->F_Type_BBLT.cond = ins_cache[1];
      instr_dec->F_Type_BBLT.op = ins_cache[2];
      instr_dec->F_Type_BBLT.h = ins_cache[23];
      instr_dec->F_Type_BBLT.offset = ins_cache[24];
      break;
    case 10:
      instr_dec->F_Type_MBXBLX.cond = ins_cache[1];
      instr_dec->F_Type_MBXBLX.op = ins_cache[2];
      instr_dec->F_Type_MBXBLX.func1 = ins_cache[3];
      instr_dec->F_Type_MBXBLX.s = ins_cache[4];
      instr_dec->F_Type_MBXBLX.one1 = ins_cache[5];
      instr_dec->F_Type_MBXBLX.one2 = ins_cache[6];
      instr_dec->F_Type_MBXBLX.one3 = ins_cache[11];
      instr_dec->F_Type_MBXBLX.subop2 = ins_cache[12];
      instr_dec->F_Type_MBXBLX.func2 = ins_cache[8];
      instr_dec->F_Type_MBXBLX.subop1 = ins_cache[9];
      instr_dec->F_Type_MBXBLX.rm = ins_cache[10];
      break;
    case 11:
      instr_dec->F_Type_MULT1.cond = ins_cache[1];
      instr_dec->F_Type_MULT1.op = ins_cache[2];
      instr_dec->F_Type_MULT1.func1 = ins_cache[3];
      instr_dec->F_Type_MULT1.s = ins_cache[4];
      instr_dec->F_Type_MULT1.rn = ins_cache[5];
      instr_dec->F_Type_MULT1.rd = ins_cache[6];
      instr_dec->F_Type_MULT1.rs = ins_cache[11];
      instr_dec->F_Type_MULT1.subop2 = ins_cache[12];
      instr_dec->F_Type_MULT1.func2 = ins_cache[8];
      instr_dec->F_Type_MULT1.subop1 = ins_cache[9];
      instr_dec->F_Type_MULT1.rm = ins_cache[10];
      break;
    case 12:
      instr_dec->F_Type_MULT2.cond = ins_cache[1];
      instr_dec->F_Type_MULT2.op = ins_cache[2];
      instr_dec->F_Type_MULT2.func1 = ins_cache[3];
      instr_dec->F_Type_MULT2.s = ins_cache[4];
      instr_dec->F_Type_MULT2.rdhi = ins_cache[5];
      instr_dec->F_Type_MULT2.rdlo = ins_cache[6];
      instr_dec->F_Type_MULT2.rs = ins_cache[11];
      instr_dec->F_Type_MULT2.subop2 = ins_cache[12];
      instr_dec->F_Type_MULT2.func2 = ins_cache[8];
      instr_dec->F_Type_MULT2.subop1 = ins_cache[9];
      instr_dec->F_Type_MULT2.rm = ins_cache[10];
      break;
    case 13:
      instr_dec->F_Type_LSI.cond = ins_cache[1];
      instr_dec->F_Type_LSI.op = ins_cache[2];
      instr_dec->F_Type_LSI.p = ins_cache[23];
      instr_dec->F_Type_LSI.u = ins_cache[25];
      instr_dec->F_Type_LSI.b = ins_cache[26];
      instr_dec->F_Type_LSI.w = ins_cache[27];
      instr_dec->F_Type_LSI.l = ins_cache[4];
      instr_dec->F_Type_LSI.rn = ins_cache[5];
      instr_dec->F_Type_LSI.rd = ins_cache[6];
      instr_dec->F_Type_LSI.imm12 = ins_cache[14];
      break;
    case 14:
      instr_dec->F_Type_LSR.cond = ins_cache[1];
      instr_dec->F_Type_LSR.op = ins_cache[2];
      instr_dec->F_Type_LSR.p = ins_cache[23];
      instr_dec->F_Type_LSR.u = ins_cache[25];
      instr_dec->F_Type_LSR.b = ins_cache[26];
      instr_dec->F_Type_LSR.w = ins_cache[27];
      instr_dec->F_Type_LSR.l = ins_cache[4];
      instr_dec->F_Type_LSR.rn = ins_cache[5];
      instr_dec->F_Type_LSR.rd = ins_cache[6];
      instr_dec->F_Type_LSR.shiftamount = ins_cache[7];
      instr_dec->F_Type_LSR.shift = ins_cache[8];
      instr_dec->F_Type_LSR.subop1 = ins_cache[9];
      instr_dec->F_Type_LSR.rm = ins_cache[10];
      break;
    case 15:
      instr_dec->F_Type_LSE.cond = ins_cache[1];
      instr_dec->F_Type_LSE.op = ins_cache[2];
      instr_dec->F_Type_LSE.p = ins_cache[23];
      instr_dec->F_Type_LSE.u = ins_cache[25];
      instr_dec->F_Type_LSE.i = ins_cache[26];
      instr_dec->F_Type_LSE.w = ins_cache[27];
      instr_dec->F_Type_LSE.l = ins_cache[4];
      instr_dec->F_Type_LSE.rn = ins_cache[5];
      instr_dec->F_Type_LSE.rd = ins_cache[6];
      instr_dec->F_Type_LSE.addr1 = ins_cache[11];
      instr_dec->F_Type_LSE.subop2 = ins_cache[12];
      instr_dec->F_Type_LSE.ss = ins_cache[15];
      instr_dec->F_Type_LSE.hh = ins_cache[28];
      instr_dec->F_Type_LSE.subop1 = ins_cache[9];
      instr_dec->F_Type_LSE.addr2 = ins_cache[10];
      break;
    case 16:
      instr_dec->F_Type_LSM.cond = ins_cache[1];
      instr_dec->F_Type_LSM.op = ins_cache[2];
      instr_dec->F_Type_LSM.p = ins_cache[23];
      instr_dec->F_Type_LSM.u = ins_cache[25];
      instr_dec->F_Type_LSM.r = ins_cache[26];
      instr_dec->F_Type_LSM.w = ins_cache[27];
      instr_dec->F_Type_LSM.l = ins_cache[4];
      instr_dec->F_Type_LSM.rn = ins_cache[5];
      instr_dec->F_Type_LSM.rlist = ins_cache[29];
      break;
    case 17:
      instr_dec->F_Type_MEMEX.cond = ins_cache[1];
      instr_dec->F_Type_MEMEX.op = ins_cache[17];
      instr_dec->F_Type_MEMEX.s = ins_cache[4];
      instr_dec->F_Type_MEMEX.rn = ins_cache[5];
      instr_dec->F_Type_MEMEX.rd = ins_cache[6];
      instr_dec->F_Type_MEMEX.subop1 = ins_cache[11];
      instr_dec->F_Type_MEMEX.func1 = ins_cache[22];
      instr_dec->F_Type_MEMEX.rt = ins_cache[10];
      break;
    case 18:
      instr_dec->F_Type_CDP.cond = ins_cache[1];
      instr_dec->F_Type_CDP.op = ins_cache[2];
      instr_dec->F_Type_CDP.subop3 = ins_cache[23];
      instr_dec->F_Type_CDP.funcc1 = ins_cache[21];
      instr_dec->F_Type_CDP.crn = ins_cache[5];
      instr_dec->F_Type_CDP.crd = ins_cache[6];
      instr_dec->F_Type_CDP.cp_num = ins_cache[11];
      instr_dec->F_Type_CDP.funcc3 = ins_cache[30];
      instr_dec->F_Type_CDP.subop1 = ins_cache[9];
      instr_dec->F_Type_CDP.crm = ins_cache[10];
      break;
    case 19:
      instr_dec->F_Type_CRT.cond = ins_cache[1];
      instr_dec->F_Type_CRT.op = ins_cache[2];
      instr_dec->F_Type_CRT.subop3 = ins_cache[23];
      instr_dec->F_Type_CRT.funcc2 = ins_cache[31];
      instr_dec->F_Type_CRT.l = ins_cache[4];
      instr_dec->F_Type_CRT.crn = ins_cache[5];
      instr_dec->F_Type_CRT.rd = ins_cache[6];
      instr_dec->F_Type_CRT.cp_num = ins_cache[11];
      instr_dec->F_Type_CRT.funcc3 = ins_cache[30];
      instr_dec->F_Type_CRT.subop1 = ins_cache[9];
      instr_dec->F_Type_CRT.crm = ins_cache[10];
      break;
    case 20:
      instr_dec->F_Type_CLS.cond = ins_cache[1];
      instr_dec->F_Type_CLS.op = ins_cache[2];
      instr_dec->F_Type_CLS.p = ins_cache[23];
      instr_dec->F_Type_CLS.u = ins_cache[25];
      instr_dec->F_Type_CLS.n = ins_cache[26];
      instr_dec->F_Type_CLS.w = ins_cache[27];
      instr_dec->F_Type_CLS.l = ins_cache[4];
      instr_dec->F_Type_CLS.rn = ins_cache[5];
      instr_dec->F_Type_CLS.crd = ins_cache[6];
      instr_dec->F_Type_CLS.cp_num = ins_cache[11];
      instr_dec->F_Type_CLS.imm8 = ins_cache[13];
      break;
    case 21:
      instr_dec->F_Type_MBKPT.cond = ins_cache[1];
      instr_dec->F_Type_MBKPT.op = ins_cache[2];
      instr_dec->F_Type_MBKPT.func1 = ins_cache[3];
      instr_dec->F_Type_MBKPT.s = ins_cache[4];
      instr_dec->F_Type_MBKPT.immediate = ins_cache[32];
      instr_dec->F_Type_MBKPT.subop2 = ins_cache[12];
      instr_dec->F_Type_MBKPT.func2 = ins_cache[8];
      instr_dec->F_Type_MBKPT.subop1 = ins_cache[9];
      instr_dec->F_Type_MBKPT.rm = ins_cache[10];
      break;
    case 22:
      instr_dec->F_Type_MSWI.cond = ins_cache[1];
      instr_dec->F_Type_MSWI.op = ins_cache[2];
      instr_dec->F_Type_MSWI.subop3 = ins_cache[23];
      instr_dec->F_Type_MSWI.swinumber = ins_cache[24];
      break;
    case 23:
      instr_dec->F_Type_MSMC.cond = ins_cache[1];
      instr_dec->F_Type_MSMC.op = ins_cache[17];
      instr_dec->F_Type_MSMC.subop1 = ins_cache[4];
      instr_dec->F_Type_MSMC.func1 = ins_cache[32];
      instr_dec->F_Type_MSMC.func2 = ins_cache[22];
      instr_dec->F_Type_MSMC.imm4 = ins_cache[10];
      break;
    case 24:
      instr_dec->F_Type_MCLZ.cond = ins_cache[1];
      instr_dec->F_Type_MCLZ.op = ins_cache[2];
      instr_dec->F_Type_MCLZ.func1 = ins_cache[3];
      instr_dec->F_Type_MCLZ.s = ins_cache[4];
      instr_dec->F_Type_MCLZ.one1 = ins_cache[5];
      instr_dec->F_Type_MCLZ.rd = ins_cache[6];
      instr_dec->F_Type_MCLZ.one3 = ins_cache[11];
      instr_dec->F_Type_MCLZ.subop2 = ins_cache[12];
      instr_dec->F_Type_MCLZ.func2 = ins_cache[8];
      instr_dec->F_Type_MCLZ.subop1 = ins_cache[9];
      instr_dec->F_Type_MCLZ.rm = ins_cache[10];
      break;
    case 25:
      instr_dec->F_Type_MMSR1.cond = ins_cache[1];
      instr_dec->F_Type_MMSR1.op = ins_cache[2];
      instr_dec->F_Type_MMSR1.func11 = ins_cache[33];
      instr_dec->F_Type_MMSR1.r = ins_cache[26];
      instr_dec->F_Type_MMSR1.func12 = ins_cache[34];
      instr_dec->F_Type_MMSR1.fieldmask = ins_cache[5];
      instr_dec->F_Type_MMSR1.rd = ins_cache[6];
      instr_dec->F_Type_MMSR1.zero3 = ins_cache[11];
      instr_dec->F_Type_MMSR1.subop2 = ins_cache[12];
      instr_dec->F_Type_MMSR1.func2 = ins_cache[8];
      instr_dec->F_Type_MMSR1.subop1 = ins_cache[9];
      instr_dec->F_Type_MMSR1.rm = ins_cache[10];
      break;
    case 26:
      instr_dec->F_Type_MMSR2.cond = ins_cache[1];
      instr_dec->F_Type_MMSR2.op = ins_cache[2];
      instr_dec->F_Type_MMSR2.func11 = ins_cache[33];
      instr_dec->F_Type_MMSR2.r = ins_cache[26];
      instr_dec->F_Type_MMSR2.func12 = ins_cache[34];
      instr_dec->F_Type_MMSR2.fieldmask = ins_cache[5];
      instr_dec->F_Type_MMSR2.one2 = ins_cache[6];
      instr_dec->F_Type_MMSR2.rotate = ins_cache[11];
      instr_dec->F_Type_MMSR2.imm8 = ins_cache[13];
      break;
    case 27:
      instr_dec->F_Type_DSPSM.cond = ins_cache[1];
      instr_dec->F_Type_DSPSM.sm = ins_cache[35];
      instr_dec->F_Type_DSPSM.drd = ins_cache[5];
      instr_dec->F_Type_DSPSM.drn = ins_cache[6];
      instr_dec->F_Type_DSPSM.rs = ins_cache[11];
      instr_dec->F_Type_DSPSM.subop2 = ins_cache[12];
      instr_dec->F_Type_DSPSM.yy = ins_cache[15];
      instr_dec->F_Type_DSPSM.xx = ins_cache[28];
      instr_dec->F_Type_DSPSM.subop1 = ins_cache[9];
      instr_dec->F_Type_DSPSM.rm = ins_cache[10];
      break;
    default:
      instr_dec->id = 0;
      break;
  }
  return instr_dec;
}
