/***************************************************************************************
* Copyright (c) 2014-2021 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __RTL_RTL_H__
#define __RTL_RTL_H__

#include <cpu/decode.h>

extern const rtlreg_t rzero;
extern rtlreg_t tmp_reg[4];

#define dsrc1 (id_src1->preg)
#define dsrc2 (id_src2->preg)
#define ddest (id_dest->preg)
#define s0    (&tmp_reg[0])
#define s1    (&tmp_reg[1])
#define s2    (&tmp_reg[2])
#define t0    (&tmp_reg[3])
#define rz (&rzero)

#define def_rtl(name, ...) void concat(rtl_, name)(Decode *s, __VA_ARGS__)

// relation operation
enum {
  //            +-- unsign
  //            |   +-- sign
  //            |   |   +-- equal
  //            |   |   |   +-- invert
  //            |   |   |   |
  RELOP_FALSE = 0 | 0 | 0 | 0,
  RELOP_TRUE  = 0 | 0 | 0 | 1,
  RELOP_EQ    = 0 | 0 | 2 | 0,
  RELOP_NE    = 0 | 0 | 2 | 1,

  RELOP_LT    = 0 | 4 | 0 | 0,
  RELOP_LE    = 0 | 4 | 2 | 0,
  RELOP_GT    = 0 | 4 | 2 | 1,
  RELOP_GE    = 0 | 4 | 0 | 1,

  RELOP_LTU   = 8 | 0 | 0 | 0,
  RELOP_LEU   = 8 | 0 | 2 | 0,
  RELOP_GTU   = 8 | 0 | 2 | 1,
  RELOP_GEU   = 8 | 0 | 0 | 1,
};

#include <rtl/fp.h>

enum {
  HOSTCALL_EXIT,  // handling nemu_trap
  HOSTCALL_INV,   // invalid opcode
  HOSTCALL_PIO,   // port I/O
  HOSTCALL_FP,    // floating point operation
  HOSTCALL_VFP,   // vector floating point operation
  HOSTCALL_MFP,   // matrix floating point operation
  HOSTCALL_CSR,   // system registers / control status registers
  HOSTCALL_TRAP,  // trap by interrupts/exceptions
  HOSTCALL_PRIV,  // privilege instructions
};

def_rtl(hostcall, uint32_t id, rtlreg_t *dest, const rtlreg_t *src1,
    const rtlreg_t *src2, word_t imm);

#include <rtl-basic.h>
#include <rtl/pseudo.h>

#endif
