#ifndef FP32_H
#define FP32_H
#endif 

#include <cstdint>
#include <string>
#include <iosfwd>
#include <iostream>

class FP32{
    FP32();

    explicit FP32(uint32_t bits); // raw bits constructor
    FP32(float value); // float constructor
    FP32(double value); // double constructor
    FP32(int value); // int constructor

    static FP32 fromBits(uint32_t bits);
    static FP32 zero(bool negative = false);
    static FP32 nan();
    static FP32 epsilon();

    // accessors 

    uint32_t bits() const{
        return bits_;
    }
    bool sign() const;
    uint8_t exponent() const;
    uint32_t mantissa() const;
    int biasedExponent() const;
    int unbiasedExponent() const;

    // classification methods

    bool isZero() const;
    bool isSubnormal() const;
    bool isNormal() const; 
    bool isInfinite() const;
    bool isNaN() const;
    bool isFinite() const;
    bool isNegative() const;

    // conversion methods 

    float toFloat() const;
    double toDouble() const;
    std::string toBinary() const;
    std::string toHex() const;

    // arithmetic operators

    FP32 operator+(const FP32& other) const;
    FP32 operator-(const FP32& other) const;
    FP32 operator*(const FP32& other) const;
    FP32 operator/(const FP32& other) const;
    FP32 operator-() const; // unary negation

    FP32& operator+=(const FP32& other);
    FP32& operator-=(const FP32& other);    
    FP32& operator*=(const FP32& other);
    FP32& operator/=(const FP32& other);

    // comparison operators

    bool operator==(const FP32& other) const;
    bool operator!=(const FP32& other) const;
    bool operator<(const FP32& other) const;
    bool operator<=(const FP32& other) const;
    bool operator>(const FP32& other) const;
    bool operator>=(const FP32& other) const;

    // mathematical functions

    FP32 abs() const;
    FP32 sqrt() const;

    // debugging 

    friend std::ostream& operator<<(std::ostream& os, const FP32& value);
    friend std::istream& operator>>(std::istream& is, FP32& value);

    private:
        uint32_t bits_; // 32 bit rep of the float 

        static constexpr uint32_t SIGN_MASK = 0x80000000;
        static constexpr uint32_t EXPONENT_MASK = 0x7F800000;
        static constexpr uint32_t MANTISSA_MASK = 0x007FFFFF;
        static constexpr uint32_t MANTISSA_BITS = 23;
        static constexpr int EXPONENT_BIAS = 127;
        static constexpr int EXPONENT_BITS = 8;
};