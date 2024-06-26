#include "wrapping_integers.hh"

using namespace std;

// Convert absolute seqno→seqno
Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  /**
   * let n = [ high_32 | low_32 ]
   * wrap(n) = low_32 + zero_point.raw_value_
   *        or low_32 + zero_point.raw_value_ - 2^{32}
   */
  return zero_point + static_cast<uint32_t>( n );
}

// Convert seqno→absolute seqno.
uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  const uint64_t n_low32 { this->raw_value_ - zero_point.raw_value_ };
  // 可以暂时使用 checkpoint 的高32bit作为返回值的高32bit，然后通过最近且不溢出的原则确定最终返回结果
  const uint64_t c_low32 { checkpoint & MASK_HIGH_32 };
  const uint64_t res {(checkpoint & MASK_HIGH_32) | n_low32};
  if(res >= BASE && n_low32 > c_low32 && (n_low32 - c_low32) > (BASE / 2)){
    return res - BASE;
  }
  if(res < MASK_HIGH_32 && c_low32 > n_low32 && (c_low32 - n_low32) > (BASE / 2)){
    return res + BASE;
  }
  return res;
} 
