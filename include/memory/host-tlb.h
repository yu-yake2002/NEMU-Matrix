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

#ifndef __MEMORY_HOST_TLB_H__
#define __MEMORY_HOST_TLB_H__

#include <common.h>

struct Decode;
word_t hosttlb_read(struct Decode *s, vaddr_t vaddr, int len, int type);
void hosttlb_read_matrix(struct Decode *s, vaddr_t vbase, vaddr_t stride,
                         int row, int column, int msew, bool transpose,
                         bool isacc, int mreg_id);
void hosttlb_write(struct Decode *s, vaddr_t vaddr, int len, word_t data);
void hosttlb_write_matrix(struct Decode *s, vaddr_t vbase, vaddr_t stride,
                          int row, int column, int msew, bool transpose,
                          bool isacc, int mreg_id);
void hosttlb_init();
void hosttlb_flush(vaddr_t vaddr);

#endif
