/***************************************************************************************
* Copyright (c) 2014-2021 Zihao Yu, Nanjing University
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

#ifndef __RTL_BASIC_H__
#define __RTL_BASIC_H__

#include "c_op.h"
#include <profiling/profiling_control.h>
#include <memory/vaddr.h>
#include <generated/autoconf.h>
/* RTL basic instructions */

#define def_rtl_compute_reg(name) \
  static inline def_rtl(name, rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  }

#define def_rtl_compute_imm(name) \
  static inline def_rtl(name ## i, rtlreg_t* dest, const rtlreg_t* src1, const sword_t imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }

#define def_rtl_compute_reg_imm(name) \
  def_rtl_compute_reg(name) \
  def_rtl_compute_imm(name) \

// compute

def_rtl_compute_reg_imm(add)
def_rtl_compute_reg_imm(sub)
def_rtl_compute_reg_imm(and)
def_rtl_compute_reg_imm(or)
def_rtl_compute_reg_imm(xor)
def_rtl_compute_reg_imm(shl)
def_rtl_compute_reg_imm(shr)
def_rtl_compute_reg_imm(sar)
def_rtl_compute_reg_imm(min)
def_rtl_compute_reg_imm(max)
def_rtl_compute_reg_imm(minu)
def_rtl_compute_reg_imm(maxu)

#ifdef CONFIG_ISA64
def_rtl_compute_reg_imm(addw)
def_rtl_compute_reg_imm(subw)
def_rtl_compute_reg_imm(shlw)
def_rtl_compute_reg_imm(shrw)
def_rtl_compute_reg_imm(sarw)
#define rtl_addiw rtl_addwi
#define rtl_shliw rtl_shlwi
#define rtl_shriw rtl_shrwi
#define rtl_sariw rtl_sarwi
#endif

static inline def_rtl(setrelop, uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2) {
  *dest = interpret_relop(relop, *src1, *src2);
}

static inline def_rtl(setrelopi, uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, sword_t imm) {
  *dest = interpret_relop(relop, *src1, imm);
}

// mul/div

def_rtl_compute_reg(mulu_lo)
def_rtl_compute_reg(mulu_hi)
def_rtl_compute_reg(muls_hi)
def_rtl_compute_reg(divu_q)
def_rtl_compute_reg(divu_r)
def_rtl_compute_reg(divs_q)
def_rtl_compute_reg(divs_r)

#ifdef CONFIG_ISA64
def_rtl_compute_reg(mulw)
def_rtl_compute_reg(divw)
def_rtl_compute_reg(divuw)
def_rtl_compute_reg(remw)
def_rtl_compute_reg(remuw)
#endif

static inline def_rtl(div64u_q, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline def_rtl(div64u_r, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline def_rtl(div64s_q, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline def_rtl(div64s_r, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend % divisor;
}

// memory

static inline def_rtl(lm, rtlreg_t *dest, const rtlreg_t* addr,
    word_t offset, int len, int mmu_mode) {
  *dest = vaddr_read(s, *addr + offset, len, mmu_mode);
#ifdef CONFIG_QUERY_REF
  cpu.query_mem_event.pc = cpu.debug.current_pc;
  cpu.query_mem_event.mem_access = true;
  cpu.query_mem_event.mem_access_is_load = true;
  cpu.query_mem_event.mem_access_vaddr = *addr + offset;
#endif
}

#ifdef CONFIG_RVMATRIX
static inline def_rtl(lmm, const uint64_t *base, const uint64_t* stride,
                      int row, int column, int msew, bool transpose,
                      int mmu_mode, bool isacc, int mreg_id) {
  vaddr_read_matrix(s, *base, *stride, row, column, msew, transpose, mmu_mode, isacc, mreg_id);
#ifdef CONFIG_QUERY_REF
  cpu.query_mem_event.pc = cpu.debug.current_pc;
  cpu.query_mem_event.mem_access = true;
  cpu.query_mem_event.mem_access_is_load = true;
  cpu.query_mem_event.mem_access_vaddr = *base;
#endif
}
#endif // CONFIG_RVMATRIX

static inline def_rtl(sm, const rtlreg_t *src1, const rtlreg_t* addr,
    word_t offset, int len, int mmu_mode) {
  vaddr_write(s, *addr + offset, len, *src1, mmu_mode);
#ifdef CONFIG_QUERY_REF
  cpu.query_mem_event.pc = cpu.debug.current_pc;
  cpu.query_mem_event.mem_access = true;
  cpu.query_mem_event.mem_access_is_load = false;
  cpu.query_mem_event.mem_access_vaddr = *addr + offset;
#endif
}

#ifdef CONFIG_RVMATRIX
static inline def_rtl(smm, const uint64_t *base, const uint64_t* stride,
                      int row, int column, int msew, bool transpose,
                      int mmu_mode, bool isacc, int mreg_id) {
  vaddr_write_matrix(s, *base, *stride, row, column, msew, transpose, mmu_mode, isacc, mreg_id);
#ifdef CONFIG_QUERY_REF
  cpu.query_mem_event.pc = cpu.debug.current_pc;
  cpu.query_mem_event.mem_access = true;
  cpu.query_mem_event.mem_access_is_load = false;
  cpu.query_mem_event.mem_access_vaddr = *base;
#endif
}
#endif // CONFIG_RVMATRIX

static inline def_rtl(lms, rtlreg_t *dest, const rtlreg_t* addr,
    word_t offset, int len, int mmu_mode) {
  word_t val = vaddr_read(s, *addr + offset, len, mmu_mode);
  switch (len) {
    case 4: *dest = (sword_t)(int32_t)val; return;
    case 1: *dest = (sword_t)( int8_t)val; return;
    case 2: *dest = (sword_t)(int16_t)val; return;
    IFDEF(CONFIG_ISA64, case 8: *dest = (sword_t)(int64_t)val; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
#ifdef CONFIG_QUERY_REF
  cpu.query_mem_event.pc = cpu.debug.current_pc;
  cpu.query_mem_event.mem_access = true;
  cpu.query_mem_event.mem_access_is_load = true;
  cpu.query_mem_event.mem_access_vaddr = *addr + offset;
#endif
}

static inline def_rtl(host_lm, rtlreg_t* dest, const void *addr, int len) {
  switch (len) {
    case 4: *dest = *(uint32_t *)addr; return;
    case 1: *dest = *( uint8_t *)addr; return;
    case 2: *dest = *(uint16_t *)addr; return;
    IFDEF(CONFIG_ISA64, case 8: *dest = *(uint64_t *)addr; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
}

static inline def_rtl(host_sm, void *addr, const rtlreg_t *src1, int len) {
  switch (len) {
    case 4: *(uint32_t *)addr = *src1; return;
    case 1: *( uint8_t *)addr = *src1; return;
    case 2: *(uint16_t *)addr = *src1; return;
    IFDEF(CONFIG_ISA64, case 8: *(uint64_t *)addr = *src1; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
}

// control
#ifndef CONFIG_SHARE
extern void simpoint_profiling(uint64_t pc, bool is_control, uint64_t abs_instr_count);
#endif // CONFIG_SHARE
extern uint64_t get_abs_instr_count();

// This rtl_j is only used in normal mode.
// rtl_j for PERF_OPT is defined as a marco in cpu-exec.c
// Note: rtl_j can ONLY be called directly in EHelper.
#ifndef CONFIG_PERF_OPT
static inline def_rtl(j, vaddr_t target) {
  // uint64_t orig_pc = cpu.pc, real_target;
#ifdef CONFIG_GUIDED_EXEC
  if(cpu.guided_exec && cpu.execution_guide.force_set_jump_target) {
    if(cpu.execution_guide.jump_target != target) {
      cpu.pc = cpu.execution_guide.jump_target;
      // printf("input jump target %lx & real jump target %lx does not match\n",
      //   cpu.execution_guide.jump_target, target
      // );
      // real_target = cpu.execution_guide.jump_target;
      goto end_of_rtl_j;
    }
  }
#endif

  cpu.pc = target;
  // real_target = target;
  // CONFIG_BR_LOG: We cannot commit br_log here, because rtl_j is used by all jump and branch.

#ifndef CONFIG_SHARE
  if (profiling_state == SimpointProfiling && workload_loaded) {
    simpoint_profiling(cpu.pc, true, get_abs_instr_count() - checkpoint_icount_base);
  }
#endif // CONFIG_SHARE

#ifdef CONFIG_GUIDED_EXEC
end_of_rtl_j:
; // make compiler happy
#endif
}
#endif // ndef CONFIG_PERF_OPT

// This rtl_jr is only used in normal mode.
// rtl_jr for PERF_OPT is defined as a marco in cpu-exec.c
// Note: rtl_jr can ONLY be called directly in EHelper.
#ifndef CONFIG_PERF_OPT
static inline def_rtl(jr, rtlreg_t *target) {
#ifdef CONFIG_BR_LOG
  uint64_t real_target;
#endif // CONFIG_BR_LOG
#ifdef CONFIG_GUIDED_EXEC
  if(cpu.guided_exec && cpu.execution_guide.force_set_jump_target) {
    if(cpu.execution_guide.jump_target != *target) {
      cpu.pc = cpu.execution_guide.jump_target;
      // printf("input jump target %lx & real jump target %lx does not match\n",
      //   cpu.execution_guide.jump_target, *target
      // );
      #ifdef CONFIG_BR_LOG
      real_target = cpu.execution_guide.jump_target;
      #endif // CONFIG_BR_LOG
      goto end_of_rtl_jr;
    }
  }
#endif

  cpu.pc = *target;
  #ifdef CONFIG_BR_LOG
  real_target = *target;
  #endif // CONFIG_BR_LOG

#ifndef CONFIG_SHARE
  if (profiling_state == SimpointProfiling && workload_loaded) {
    simpoint_profiling(cpu.pc, true, get_abs_instr_count() - checkpoint_icount_base);
  }
#endif // CONFIG_SHARE

#ifdef CONFIG_GUIDED_EXEC
end_of_rtl_jr:
; // make compiler happy
#endif

  IFDEF(CONFIG_BR_LOG, br_log_commit(s->pc, real_target, 1, BR_JUMP));
}
#endif // ndef CONFIG_PERF_OPT

// This rtl_jrelop is only used in normal mode.
// rtl_jrelop for PERF_OPT is defined as a marco in cpu-exec.c
// Note: rtl_jrelop can ONLY be called directly in EHelper.
#ifndef CONFIG_PERF_OPT
static inline def_rtl(jrelop, uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
  bool is_jmp = interpret_relop(relop, *src1, *src2);
  IFDEF(CONFIG_BR_LOG, br_log_commit(s->pc, target, is_jmp, BR_BRANCH));
  rtl_j(s, (is_jmp ? target : s->snpc));
}
#endif // ndef CONFIG_PERF_OPT

//#include "rtl-fp.h"
#endif
