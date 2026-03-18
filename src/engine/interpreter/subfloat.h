#ifndef __INTERPRETER_SUBFLOAT_H__
#define __INTERPRETER_SUBFLOAT_H__

#include <common.h>

typedef struct {
  uint8_t v;
} float8e4m3_t;

typedef struct {
  uint8_t v;
} float8e5m2_t;

typedef struct {
  uint8_t payload;
  uint8_t scale_e8m0;
} mxfp8_elem_t;

bool is_powprec_width(uint32_t w);
rtlreg_t subfloat_compute_bin(uint32_t op, uint32_t w, rtlreg_t src1, rtlreg_t src2);
rtlreg_t subfloat_compute_madd(uint32_t w, rtlreg_t acc, rtlreg_t src1, rtlreg_t src2);

#endif
