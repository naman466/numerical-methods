#include "FP32.h"
#include <algorithm>

uint32_t FP32::roundToNearest(uint64_t value, int shift) {
    if (shift <= 0) return static_cast<uint32_t>(value); // round to nearest, bankers rounding

    uint64_t mask = (1ULL << shift) - 1;
    uint64_t halfway = 1ULL << (shift - 1);
    uint64_t remainder = value & mask;
    uint64_t result = value >> shift;

    if (remainder > halfway){
        result++; // round up
    }
    else if (remainder == halfway){
        // tie, round to even (lsb = 0)
        if (result & 1){
            result++; 
        }
    }

    return static_cast<uint32_t>(result);
}

// TODO: BUG - normalization logic is zeroing out results
FP32 FP32::normalize(bool sign, int exp, uint64_t significand) {
    if (significand == 0){
        return FP32::zero(sign);
    }

    // find leading 1 position
    int leading_bit = 63;
    while (leading_bit >= 0 && !((significand >> leading_bit) & 1)) {
        leading_bit--;
    }

    // adjust exponent based on leading bit pos
    // for normalized numbers, we want leading bit at 23
    int shift = leading_bit - MANTISSA_BITS;
    exp += shift;

    // check overflow/underflow
    if (exp >= 255) {
        return FP32::infinity(sign);
    }

    if (exp <= 0) {
        // subnormal or underflow
        int denorm_shift = 1 - exp;
        if (denorm_shift > 63){
            return FP32::zero(sign); 
        }

        uint32_t mant = roundToNearest(significand, denorm_shift + shift);
        return FP32(packBits(sign, 0, mant));
    }
}

FP32 FP32::addImpl(const FP32& a, const FP32& b) {
    // handle special cases first
    if (a.isNaN() || b.isNaN()) return FP32::nan(); 
    
    if (a.isInfinity()) {
        if (b.isInfinity() && a.sign() != b.sign()) {
            return FP32::nan(); // inf - inf = NaN
        }
        return a;
    }
    
    if (b.isInfinity()) return b;
    
    if (a.isZero()) return b;
    if (b.isZero()) return a;
    
    // extract components
    bool sign_a = a.sign();
    bool sign_b = b.sign();
    int exp_a = a.unbiasedExponent();
    int exp_b = b.unbiasedExponent();
    
    // get significands with implicit bit
    uint64_t sig_a = a.mantissa();
    uint64_t sig_b = b.mantissa();
    
    if (a.isNormal()) sig_a |= (1ULL << MANTISSA_BITS);
    if (b.isNormal()) sig_b |= (1ULL << MANTISSA_BITS);
    
    // align significands
    sig_a <<= 3;
    sig_b <<= 3;
    
    // make exponents equal by shifitng smaller one
    if (exp_a > exp_b) {
        int diff = exp_a - exp_b;
        if (diff > 31) {
            sig_b = 0;
        } else {
            sig_b >>= diff;
        }
        exp_b = exp_a;
    } else if (exp_b > exp_a) {
        int diff = exp_b - exp_a;
        if (diff > 31) {
            sig_a = 0;
        } else {
            sig_a >>= diff;
        }
        exp_a = exp_b;
    }
    
    // perform add or sub
    uint64_t result_sig;
    bool result_sign;
    
    if (sign_a == sign_b) {
        // same sign = add
        result_sig = sig_a + sig_b;
        result_sign = sign_a;
    } else {
        // different sign = sub
        if (sig_a >= sig_b) {
            result_sig = sig_a - sig_b;
            result_sign = sign_a;
        } else {
            result_sig = sig_b - sig_a;
            result_sign = sign_b;
        }
    }
    
    // normalize result
    return normalize(result_sign, exp_a + EXPONENT_BIAS - 3, result_sig);
}

FP32 FP32::operator+(const FP32& other) const {
    return addImpl(*this, other);
}

FP32 FP32::operator-(const FP32& other) const {
    return addImpl(*this, -other);
}

FP32& FP32::operator+=(const FP32& other) {
    *this = *this + other;
    return *this;
}

FP32& FP32::operator-=(const FP32& other) {
    *this = *this - other;
    return *this;
}

FP32 FP32::multiplyImpl(const FP32& a, const FP32& b) {
    // handle special cases
    if (a.isNaN() || b.isNaN()) return FP32::nan();
    
    if (a.isInfinity() || b.isInfinity()) {
        if (a.isZero() || b.isZero()) {
            return FP32::nan(); // 0 * inf = NaN
        }
        return FP32::infinity(a.sign() != b.sign());
    }
    
    if (a.isZero() || b.isZero()) {
        return FP32::zero(a.sign() != b.sign());
    }
    
    // extract components
    bool result_sign = a.sign() != b.sign();
    int exp_a = a.unbiasedExponent();
    int exp_b = b.unbiasedExponent();
    
    // get significands
    uint64_t sig_a = a.mantissa();
    uint64_t sig_b = b.mantissa();
    
    if (a.isNormal()) sig_a |= (1ULL << MANTISSA_BITS);
    if (b.isNormal()) sig_b |= (1ULL << MANTISSA_BITS);
    
    // Multiply significands
    // sig_a and sig_b are 24-bit fixed-point 
    // product will be 48-bit fixed-point
    uint64_t result_sig = sig_a * sig_b;
    
   
    // input_exp = (exp_a + exp_b + BIAS) - expected_shift
    int result_exp = exp_a + exp_b + EXPONENT_BIAS - MANTISSA_BITS;
    
    return normalize(result_sign, result_exp, result_sig);
}

FP32 FP32::operator*(const FP32& other) const {
    return multiplyImpl(*this, other);
}

FP32& FP32::operator*=(const FP32& other) {
    *this = *this * other;
    return *this;
}

FP32 FP32::operator/(const FP32& other) const {
    // handle special cases
    if (isNaN() || other.isNaN()) return FP32::nan();
    
    if (isInfinity()) {
        if (other.isInfinity()) {
            return FP32::nan(); // inf / inf = NaN
        }
        return FP32::infinity(sign() != other.sign());
    }
    
    if (other.isInfinity()) {
        return FP32::zero(sign() != other.sign());
    }
    
    if (other.isZero()) {
        if (isZero()) {
            return FP32::nan(); // 0 / 0 = NaN
        }
        return FP32::infinity(sign() != other.sign());
    }
    
    if (isZero()) {
        return FP32::zero(sign() != other.sign());
    }
    
    // extract components
    bool result_sign = sign() != other.sign();
    int exp_a = unbiasedExponent();
    int exp_b = other.unbiasedExponent();
    
    // get significands 
    uint64_t sig_a = mantissa();
    uint64_t sig_b = other.mantissa();
    
    if (isNormal()) sig_a |= (1ULL << MANTISSA_BITS);
    if (other.isNormal()) sig_b |= (1ULL << MANTISSA_BITS);
    
    // shift dividend left for precision
    sig_a <<= MANTISSA_BITS;
    
    // divide significands
    uint64_t result_sig = sig_a / sig_b;
    
   
    // result_exp = exp_a - exp_b + BIAS
    int result_exp = exp_a - exp_b + EXPONENT_BIAS;
    
    // normalize
    return normalize(result_sign, result_exp, result_sig);
}

FP32& FP32::operator/=(const FP32& other) {
    *this = *this / other;
    return *this;
}


FP32 FP32::operator-() const {
    // flip the sign bit
    return FP32(bits_ ^ SIGN_MASK);
}




