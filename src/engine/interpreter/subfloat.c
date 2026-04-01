#include <rtl/rtl.h>
#ifdef CONFIG_FPU_SOFT
#include <softfloat.h>
#include <specialize.h>
#include <internals.h>
#else
#include "host-fp.h"
#endif
#include "subfloat.h"

static inline float32_t sub_u32_to_f32(uint32_t u) {
  float32_t f = {.v = u};
  return f;
}

static inline uint32_t sub_f32_to_u32(float32_t f) {
  return f.v;
}

static inline uint16_t sub_f32_to_f16_bits(float32_t in) {
#ifdef CONFIG_FPU_SOFT
  return f32_to_f16(in).v;
#else
  panic("subfloat fp16 conversion requires CONFIG_FPU_SOFT");
  return 0;
#endif
}

static inline float32_t sub_f16_bits_to_f32(uint16_t in) {
#ifdef CONFIG_FPU_SOFT
  return f16_to_f32((float16_t){.v = in});
#else
  panic("subfloat fp16 conversion requires CONFIG_FPU_SOFT");
  return sub_u32_to_f32(0);
#endif
}

static inline float32_t f8e4m3_to_f32(float8e4m3_t x) {
  uint32_t sign = (x.v >> 7) & 1u;
  uint32_t exp = (x.v >> 3) & 0xfu;
  uint32_t frac = x.v & 0x7u;
  uint32_t out;

  if (exp == 0) {
    if (frac == 0) {
      out = sign << 31;
    } else {
      int e = -6;
      uint32_t sig = frac;
      while ((sig & 0x8u) == 0) {
        sig <<= 1;
        e--;
      }
      sig &= 0x7u;
      out = (sign << 31) | ((uint32_t)(e + 127) << 23) | (sig << 20);
    }
  } else if (exp == 0xfu) {
    if (frac == 0) {
      out = (sign << 31) | 0x7f800000u;
    } else {
      out = 0x7fc00000u;
    }
  } else {
    int e = (int)exp - 7;
    out = (sign << 31) | ((uint32_t)(e + 127) << 23) | (frac << 20);
  }

  return sub_u32_to_f32(out);
}

static inline float32_t f8e5m2_to_f32(float8e5m2_t x) {
  uint32_t sign = (x.v >> 7) & 1u;
  uint32_t exp = (x.v >> 2) & 0x1fu;
  uint32_t frac = x.v & 0x3u;
  uint32_t out;

  if (exp == 0) {
    if (frac == 0) {
      out = sign << 31;
    } else {
      int e = -14;
      uint32_t sig = frac;
      while ((sig & 0x4u) == 0) {
        sig <<= 1;
        e--;
      }
      sig &= 0x3u;
      out = (sign << 31) | ((uint32_t)(e + 127) << 23) | (sig << 21);
    }
  } else if (exp == 0x1fu) {
    if (frac == 0) {
      out = (sign << 31) | 0x7f800000u;
    } else {
      out = 0x7fc00000u;
    }
  } else {
    int e = (int)exp - 15;
    out = (sign << 31) | ((uint32_t)(e + 127) << 23) | (frac << 21);
  }

  return sub_u32_to_f32(out);
}

static inline float8e4m3_t f32_to_f8e4m3(float32_t in) {
  uint32_t x = in.v;
  uint32_t sign = (x >> 31) & 1u;
  uint32_t exp = (x >> 23) & 0xffu;
  uint32_t frac = x & 0x7fffffu;
  float8e4m3_t out;

  if (exp == 0xffu) {
    out.v = (frac == 0) ? (uint8_t)((sign << 7) | 0x78u) : 0x7fu;
    return out;
  }

  if (exp == 0 && frac == 0) {
    out.v = (uint8_t)(sign << 7);
    return out;
  }

  int e = (int)exp - 127;
  int e4 = e + 7;

  if (e4 >= 0xf) {
    out.v = (uint8_t)((sign << 7) | 0x78u);
    return out;
  }

  if (e4 <= 0) {
    if (e4 < -3) {
      out.v = (uint8_t)(sign << 7);
      return out;
    }
    uint32_t m = frac | 0x800000u;
    int rshift = 21 + (1 - e4);
    uint32_t frac3 = m >> rshift;
    uint32_t rem = m & ((1u << rshift) - 1);
    uint32_t half = 1u << (rshift - 1);
    if (rem > half || (rem == half && (frac3 & 1u))) frac3++;
    if (frac3 >= 8) {
      out.v = (uint8_t)((sign << 7) | 0x08u);
      return out;
    }
    out.v = (uint8_t)((sign << 7) | (frac3 & 0x7u));
    return out;
  }

  uint32_t frac3 = frac >> 20;
  uint32_t rem = frac & ((1u << 20) - 1);
  uint32_t half = 1u << 19;
  if (rem > half || (rem == half && (frac3 & 1u))) frac3++;
  if (frac3 >= 8) {
    frac3 = 0;
    e4++;
    if (e4 >= 0xf) {
      out.v = (uint8_t)((sign << 7) | 0x78u);
      return out;
    }
  }
  out.v = (uint8_t)((sign << 7) | ((uint32_t)e4 << 3) | (frac3 & 0x7u));
  return out;
}

static inline float8e5m2_t f32_to_f8e5m2(float32_t in) {
  uint32_t x = in.v;
  uint32_t sign = (x >> 31) & 1u;
  uint32_t exp = (x >> 23) & 0xffu;
  uint32_t frac = x & 0x7fffffu;
  float8e5m2_t out;

  if (exp == 0xffu) {
    out.v = (frac == 0) ? (uint8_t)((sign << 7) | 0x7cu) : 0x7fu;
    return out;
  }

  if (exp == 0 && frac == 0) {
    out.v = (uint8_t)(sign << 7);
    return out;
  }

  int e = (int)exp - 127;
  int e5 = e + 15;

  if (e5 >= 0x1f) {
    out.v = (uint8_t)((sign << 7) | 0x7cu);
    return out;
  }

  if (e5 <= 0) {
    if (e5 < -2) {
      out.v = (uint8_t)(sign << 7);
      return out;
    }
    uint32_t m = frac | 0x800000u;
    int rshift = 21 + (1 - e5);
    uint32_t frac2 = m >> rshift;
    uint32_t rem = m & ((1u << rshift) - 1);
    uint32_t half = 1u << (rshift - 1);
    if (rem > half || (rem == half && (frac2 & 1u))) frac2++;
    if (frac2 >= 4) {
      out.v = (uint8_t)((sign << 7) | 0x04u);
      return out;
    }
    out.v = (uint8_t)((sign << 7) | (frac2 & 0x3u));
    return out;
  }

  uint32_t frac2 = frac >> 21;
  uint32_t rem = frac & ((1u << 21) - 1);
  uint32_t half = 1u << 20;
  if (rem > half || (rem == half && (frac2 & 1u))) frac2++;
  if (frac2 >= 4) {
    frac2 = 0;
    e5++;
    if (e5 >= 0x1f) {
      out.v = (uint8_t)((sign << 7) | 0x7cu);
      return out;
    }
  }
  out.v = (uint8_t)((sign << 7) | ((uint32_t)e5 << 2) | (frac2 & 0x3u));
  return out;
}

static inline mxfp8_elem_t unpack_mxfp8(rtlreg_t src) {
  mxfp8_elem_t e;
  e.payload = (uint8_t)(src & 0xffu);
  e.scale_e8m0 = (uint8_t)((src >> 8) & 0xffu);
  return e;
}

static inline float32_t e8m0_to_f32(uint8_t x) {
  if (x == 0xffu) {
    // OCP MX: if scale is NaN, all values in the block are NaN.
    return sub_u32_to_f32(0x7fc00000u);
  }
  // E8M0 encodes powers-of-two in range 2^-127 .. 2^127.
  // x=0 maps to 2^-127, which is subnormal in float32.
  if (x == 0) return sub_u32_to_f32(0x00400000u);
  return sub_u32_to_f32((uint32_t)x << 23);
}

static inline float32_t decode_mxfp8_to_f32(uint32_t w, rtlreg_t src) {
  mxfp8_elem_t e = unpack_mxfp8(src);
  float32_t payload = (w == FPCALL_MXFP8_E5M2_to_16 || w == FPCALL_MXFP8_E5M2_to_32 || w == FPCALL_MXFP8_E5M2)
      ? f8e5m2_to_f32((float8e5m2_t){.v = e.payload})
      : f8e4m3_to_f32((float8e4m3_t){.v = e.payload});

  if (e.scale_e8m0 == 0xffu) {
    return e8m0_to_f32(e.scale_e8m0);
  }

  return f32_mul(payload, e8m0_to_f32(e.scale_e8m0));
}

static inline float32_t sub_f32_min(float32_t a, float32_t b) {
#ifdef CONFIG_FPU_SOFT
  bool less = f32_lt_quiet(a, b) || (f32_eq(a, b) && (a.v & ((uint32_t)1u << 31)));
  if (isNaNF32UI(a.v) && isNaNF32UI(b.v)) return sub_u32_to_f32(defaultNaNF32UI);
  return (less || isNaNF32UI(b.v)) ? a : b;
#else
  return f32_min(a, b);
#endif
}

static inline float32_t sub_f32_max(float32_t a, float32_t b) {
#ifdef CONFIG_FPU_SOFT
  bool greater = f32_lt_quiet(b, a) || (f32_eq(b, a) && (b.v & ((uint32_t)1u << 31)));
  if (isNaNF32UI(a.v) && isNaNF32UI(b.v)) return sub_u32_to_f32(defaultNaNF32UI);
  return (greater || isNaNF32UI(b.v)) ? a : b;
#else
  return f32_max(a, b);
#endif
}

static inline float32_t decode_subfloat(uint32_t w, rtlreg_t src) {
  switch (w) {
    case FPCALL_BF16:
    case FPCALL_BF16_to_32:
      return bf16_to_f32((bfloat16_t){.v = (uint16_t)src});
    case FPCALL_E4M3:
    case FPCALL_E4M3_to_16:
    case FPCALL_E4M3_to_32:
      return f8e4m3_to_f32((float8e4m3_t){.v = (uint8_t)src});
    case FPCALL_E5M2:
    case FPCALL_E5M2_to_16:
    case FPCALL_E5M2_to_32:
      return f8e5m2_to_f32((float8e5m2_t){.v = (uint8_t)src});
    case FPCALL_MXFP8_E4M3:
    case FPCALL_MXFP8_E4M3_to_16:
    case FPCALL_MXFP8_E4M3_to_32:
    case FPCALL_MXFP8_E5M2:
    case FPCALL_MXFP8_E5M2_to_16:
    case FPCALL_MXFP8_E5M2_to_32:
      return decode_mxfp8_to_f32(w, src);
    default:
      panic("unsupported subfloat w=%u", w);
  }
}

static inline float32_t decode_subfloat_acc(uint32_t w, rtlreg_t acc) {
  switch (w) {
    case FPCALL_BF16_to_32:
    case FPCALL_E4M3_to_32:
    case FPCALL_E5M2_to_32:
    case FPCALL_MXFP8_E4M3_to_32:
    case FPCALL_MXFP8_E5M2_to_32:
      return sub_u32_to_f32((uint32_t)acc);
    case FPCALL_E4M3_to_16:
    case FPCALL_E5M2_to_16:
    case FPCALL_MXFP8_E4M3_to_16:
    case FPCALL_MXFP8_E5M2_to_16:
      return sub_f16_bits_to_f32((uint16_t)acc);
    default:
      return decode_subfloat(w, acc);
  }
}

static inline rtlreg_t encode_subfloat(uint32_t w, float32_t val) {
  switch (w) {
    case FPCALL_BF16:
      return f32_to_bf16(val).v;
    case FPCALL_BF16_to_32:
      return sub_f32_to_u32(val);
    case FPCALL_E4M3:
      return f32_to_f8e4m3(val).v;
    case FPCALL_E4M3_to_16:
      return sub_f32_to_f16_bits(val);
    case FPCALL_E4M3_to_32:
      return sub_f32_to_u32(val);
    case FPCALL_E5M2:
      return f32_to_f8e5m2(val).v;
    case FPCALL_E5M2_to_16:
      return sub_f32_to_f16_bits(val);
    case FPCALL_E5M2_to_32:
      return sub_f32_to_u32(val);
    case FPCALL_MXFP8_E4M3_to_16:
    case FPCALL_MXFP8_E5M2_to_16:
      return sub_f32_to_f16_bits(val);
    case FPCALL_MXFP8_E4M3_to_32:
    case FPCALL_MXFP8_E5M2_to_32:
      return sub_f32_to_u32(val);
    case FPCALL_MXFP8_E4M3:
    case FPCALL_MXFP8_E5M2:
      panic("mxfp8 element re-quantization is not enabled yet (no pack instruction)");
    default:
      panic("unsupported subfloat w=%u", w);
  }
}

bool is_powprec_width(uint32_t w) {
  switch (w) {
    case FPCALL_BF16_to_32:
    case FPCALL_E4M3:
    case FPCALL_E4M3_to_16:
    case FPCALL_E4M3_to_32:
    case FPCALL_E5M2:
    case FPCALL_E5M2_to_16:
    case FPCALL_E5M2_to_32:
    case FPCALL_MXFP8_E4M3:
    case FPCALL_MXFP8_E4M3_to_16:
    case FPCALL_MXFP8_E4M3_to_32:
    case FPCALL_MXFP8_E5M2:
    case FPCALL_MXFP8_E5M2_to_16:
    case FPCALL_MXFP8_E5M2_to_32:
      return true;
    default:
      return false;
  }
}

rtlreg_t subfloat_compute_bin(uint32_t op, uint32_t w, rtlreg_t src1, rtlreg_t src2) {
  float32_t a = decode_subfloat(w, src1);
  float32_t b = decode_subfloat(w, src2);
  float32_t out;

  switch (op) {
    case FPCALL_ADD: out = f32_add(a, b); break;
    case FPCALL_SUB: out = f32_sub(a, b); break;
    case FPCALL_MUL: out = f32_mul(a, b); break;
    case FPCALL_DIV: out = f32_div(a, b); break;
    case FPCALL_MIN: out = sub_f32_min(a, b); break;
    case FPCALL_MAX: out = sub_f32_max(a, b); break;
    default: panic("unsupported subfloat op=%u", op);
  }

  return encode_subfloat(w, out);
}

rtlreg_t subfloat_compute_madd(uint32_t w, rtlreg_t acc, rtlreg_t src1, rtlreg_t src2) {
  float32_t a = decode_subfloat(w, src1);
  float32_t b = decode_subfloat(w, src2);
  float32_t c = decode_subfloat_acc(w, acc);
  float32_t out = f32_mulAdd(a, b, c);
  return encode_subfloat(w, out);
}
