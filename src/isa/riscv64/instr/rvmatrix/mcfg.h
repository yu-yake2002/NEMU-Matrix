/***************************************************************************************
* Copyright (c) 2020-2022 Institute of Computing Technology, Chinese Academy of Sciences
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

#include <common.h>

#ifdef CONFIG_RVMATRIX

#include "cpu/exec.h"
#include "../local-include/csr.h"
#include "../local-include/intr.h"
#include "../local-include/rtl.h"
#include "../local-include/reg.h"
#include <stdio.h>

def_EHelper(msettype) {
  // read mtype value from rs1
  s->src1.val = reg_l(s->src1.reg);
  // write mtype value to mtype_csr_reg and rd
  mtype->val = s->src1.val;
  mtype->mill = (mtype->pad != 0); // TODO: add other illegal cases
  reg_l(s->dest.reg) = mtype->val;
}

def_EHelper(msettypei) {
  mtype->val = ((mtype->val >> 10) << 10) | s->src2.imm;
  mtype->mill = (mtype->pad != 0); // TODO: add other illegal cases
  reg_l(s->dest.reg) = mtype->val;
}

def_EHelper(msettypehi) {
  mtype->val = (mtype->val & 0x3ff) | (s->src2.imm << 10);
  mtype->mill = (mtype->pad != 0); // TODO: add other illegal cases
  reg_l(s->dest.reg) = mtype->val;
}

def_EHelper(msettilem) {
  s->src1.val = reg_l(s->src1.reg);
  if (s->src1.reg) {
    if (s->src1.val <= TMMAX) {
      mtilem->val = s->src1.val;
    } else {
      mtilem->val = TMMAX;
    }
  } else if (s->dest.reg) {
    mtilem->val = TMMAX;
  }
  reg_l(s->dest.reg) = mtilem->val;
}

def_EHelper(msettilemi) {
  if (s->src2.imm <= TMMAX) {
    mtilem->val = s->src2.imm;
  } else {
    mtilem->val = TMMAX;
  }
  reg_l(s->dest.reg) = mtilem->val;
}

def_EHelper(msettilek) {
  s->src1.val = reg_l(s->src1.reg);
  int SEW = s->m_width * 8;
  if (s->src1.reg) {
    if (s->src1.val <= TKMAX(SEW)) {
      mtilek->val = s->src1.val;
    } else {
      mtilek->val = TKMAX(SEW);
    }
  } else if (s->dest.reg) {
    mtilek->val = TKMAX(SEW);
  }
  reg_l(s->dest.reg) = mtilek->val;
}

def_EHelper(msettileki) {
  int SEW = s->m_width * 8;
  if (s->src2.imm <= TKMAX(SEW)) {
    mtilek->val = s->src2.imm;
  } else {
    mtilek->val = TKMAX(SEW);
  }
  reg_l(s->dest.reg) = mtilek->val;
}

def_EHelper(msettilen) {
  s->src1.val = reg_l(s->src1.reg);
  int SEW = s->m_width * 8;
  if (s->src1.reg) {
    if (s->src1.val <= TNMAX(SEW)) {
      mtilen->val = s->src1.val;
    } else {
      mtilen->val = TNMAX(SEW);
    }
  } else if (s->dest.reg) {
    mtilen->val = TNMAX(SEW);
  }
  reg_l(s->dest.reg) = mtilen->val;
}

def_EHelper(msettileni) {
  int SEW = s->m_width * 8;
  if (s->src2.imm <= TNMAX(SEW)) {
    mtilen->val = s->src2.imm;
  } else {
    mtilen->val = TNMAX(SEW);
  }
  reg_l(s->dest.reg) = mtilen->val;
}

#endif // CONFIG_RVMATRIX