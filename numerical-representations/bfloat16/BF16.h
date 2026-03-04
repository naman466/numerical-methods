#ifndef BFLOAT16_H
#define BFLOAT16_H

#include <cstdint>
#include <string>
#include <iosfwd>
#include <iostream>

class BFloat16 {
public:
    // constructors 

    BFloat16();                          // default
    explicit BFloat16(uint16_t bits);    // raw bits
    BFloat16(float value);               // native float
    BFloat16(double value);              // native double
    BFloat16(int value);                 // integer
    
    // named constructors

    static BFloat16 fromBits(uint16_t bits);
    static BFloat16 zero(bool negative = false);
    static BFloat16 infinity(bool negative = false);
    static BFloat16 nan();
    static BFloat16 epsilon();
    
    // accessors

    uint16_t bits() const { return bits_; }
    bool sign() const;
    uint8_t exponent() const;
    uint8_t mantissa() const;
    int biasedExponent() const;
    int unbiasedExponent() const;
    
    // classification

    bool isZero() const;
    bool isInfinity() const;
    bool isNaN() const;
    bool isNormal() const;
    bool isSubnormal() const;
    bool isFinite() const;
    bool isNegative() const;
    
    // conversion

    float toFloat() const;
    double toDouble() const;
    std::string toBinary() const;
    std::string toHex() const;
    
    // arithmetic operators

    BFloat16 operator+(const BFloat16& other) const;
    BFloat16 operator-(const BFloat16& other) const;
    BFloat16 operator*(const BFloat16& other) const;
    BFloat16 operator/(const BFloat16& other) const;
    BFloat16 operator-() const;  // unary negation
    
    BFloat16& operator+=(const BFloat16& other);
    BFloat16& operator-=(const BFloat16& other);
    BFloat16& operator*=(const BFloat16& other);
    BFloat16& operator/=(const BFloat16& other);
    
    // comparison operators

    bool operator==(const BFloat16& other) const;
    bool operator!=(const BFloat16& other) const;
    bool operator<(const BFloat16& other) const;
    bool operator<=(const BFloat16& other) const;
    bool operator>(const BFloat16& other) const;
    bool operator>=(const BFloat16& other) const;
    
    // mathematical functions

    BFloat16 abs() const;
    BFloat16 sqrt() const;
    
    // i/o

    friend std::ostream& operator<<(std::ostream& os, const BFloat16& bf);
    friend std::istream& operator>>(std::istream& is, BFloat16& bf);
    
    // debugging
    void printDetails(std::ostream& os = std::cout) const;
    std::string getComponentsString() const;
    
    // conversion to / from FP32

    uint32_t toFP32Bits() const;
    static BFloat16 fromFP32Bits(uint32_t fp32_bits);
    
private:
    uint16_t bits_;  // actual representation
    
    // constants

    static constexpr uint16_t SIGN_MASK     = 0x8000u;
    static constexpr uint16_t EXPONENT_MASK = 0x7F80u;
    static constexpr uint16_t MANTISSA_MASK = 0x007Fu;
    static constexpr int EXPONENT_BIAS      = 127;
    static constexpr int MANTISSA_BITS      = 7;
    static constexpr int EXPONENT_BITS      = 8;
    
    // helper functions

    static uint16_t packBits(bool sign, uint8_t exp, uint8_t mant);
    static BFloat16 addImpl(const BFloat16& a, const BFloat16& b);
    static BFloat16 multiplyImpl(const BFloat16& a, const BFloat16& b);
    static int compareImpl(const BFloat16& a, const BFloat16& b);
    static BFloat16 normalize(bool sign, int exp, uint32_t significand);
    static uint16_t roundToNearest(uint32_t value, int shift);
};

// non member finctions
BFloat16 abs(const BFloat16& x);
BFloat16 sqrt(const BFloat16& x);

#endif // 