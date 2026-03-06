#include "BF16.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cstring>

void testConstruction() {
    std::cout << "\nTesting Construction" << std::endl;
    
    BFloat16 zero;
    std::cout << "Default (zero): " << zero << std::endl;
    zero.printDetails();
    
    BFloat16 one(1.0f);
    std::cout << "\nBFloat16(1.0f): " << one << std::endl;
    one.printDetails();
    
    BFloat16 neg_half(-0.5f);
    std::cout << "\nBFloat16(-0.5f): " << neg_half << std::endl;
    neg_half.printDetails();
    
    BFloat16 from_bits = BFloat16::fromBits(0x3F80); // 1.0
    std::cout << "\nFrom bits 0x3F80: " << from_bits << std::endl;
}

void testSpecialValues() {
    std::cout << "\nTesting Special Values" << std::endl;
    
    BFloat16 pos_inf = BFloat16::infinity(false);
    BFloat16 neg_inf = BFloat16::infinity(true);
    BFloat16 nan = BFloat16::nan();
    BFloat16 pos_zero = BFloat16::zero(false);
    BFloat16 neg_zero = BFloat16::zero(true);
    BFloat16 epsilon = BFloat16::epsilon();
    
    std::cout << "Positive infinity: " << pos_inf << " (bits: " << pos_inf.toHex() << ")" << std::endl;
    std::cout << "Negative infinity: " << neg_inf << " (bits: " << neg_inf.toHex() << ")" << std::endl;
    std::cout << "NaN: " << nan << " (bits: " << nan.toHex() << ")" << std::endl;
    std::cout << "Positive zero: " << pos_zero << " (bits: " << pos_zero.toHex() << ")" << std::endl;
    std::cout << "Negative zero: " << neg_zero << " (bits: " << neg_zero.toHex() << ")" << std::endl;
    std::cout << "Epsilon: " << epsilon << " (bits: " << epsilon.toHex() << ")" << std::endl;
    
    std::cout << "\nClassification tests:" << std::endl;
    std::cout << "pos_inf.isInfinity(): " << pos_inf.isInfinity() << std::endl;
    std::cout << "nan.isNaN(): " << nan.isNaN() << std::endl;
    std::cout << "pos_zero.isZero(): " << pos_zero.isZero() << std::endl;
    std::cout << "neg_zero.isZero(): " << neg_zero.isZero() << std::endl;
    std::cout << "BFloat16(1.0).isNormal(): " << BFloat16(1.0f).isNormal() << std::endl;
}

void testArithmetic() {
    std::cout << "\nTesting Arithmetic" << std::endl;
    
    BFloat16 a(2.5f);
    BFloat16 b(1.5f);
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    
    BFloat16 sum = a + b;
    std::cout << "a + b = " << sum << " (expected: 4.0)" << std::endl;
    
    BFloat16 diff = a - b;
    std::cout << "a - b = " << diff << " (expected: 1.0)" << std::endl;
    
    BFloat16 prod = a * b;
    std::cout << "a * b = " << prod << " (expected: 3.75)" << std::endl;
    
    BFloat16 quot = a / b;
    std::cout << "a / b = " << quot << " (expected: 1.666...)" << std::endl;
    
    BFloat16 neg_a = -a;
    std::cout << "-a = " << neg_a << " (expected: -2.5)" << std::endl;
    
    // test with larger values
    std::cout << "\nLarger value tests:" << std::endl;
    BFloat16 large1(1000.0f);
    BFloat16 large2(2000.0f);
    std::cout << large1 << " + " << large2 << " = " << (large1 + large2) << std::endl;
    std::cout << large2 << " - " << large1 << " = " << (large2 - large1) << std::endl;
}

void testComparison() {
    std::cout << "\nTesting Comparison" << std::endl;
    
    BFloat16 a(2.0f);
    BFloat16 b(3.0f);
    BFloat16 c(2.0f);
    
    std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
    std::cout << "a == c: " << (a == c) << " (expected: 1)" << std::endl;
    std::cout << "a != b: " << (a != b) << " (expected: 1)" << std::endl;
    std::cout << "a < b: " << (a < b) << " (expected: 1)" << std::endl;
    std::cout << "b > a: " << (b > a) << " (expected: 1)" << std::endl;
    std::cout << "a <= c: " << (a <= c) << " (expected: 1)" << std::endl;
    std::cout << "a >= c: " << (a >= c) << " (expected: 1)" << std::endl;
    
    // test with negative numbers
    BFloat16 neg(-5.0f);
    BFloat16 pos(5.0f);
    std::cout << "\nneg = " << neg << ", pos = " << pos << std::endl;
    std::cout << "neg < pos: " << (neg < pos) << " (expected: 1)" << std::endl;
    
    // test zero equality
    BFloat16 pos_zero = BFloat16::zero(false);
    BFloat16 neg_zero = BFloat16::zero(true);
    std::cout << "\npos_zero == neg_zero: " << (pos_zero == neg_zero) << " (expected: 1)" << std::endl;
}

void testMathFunctions() {
    std::cout << "\nTesting Math Functions" << std::endl;
    
    BFloat16 neg(-5.5f);
    BFloat16 abs_val = neg.abs();
    std::cout << "abs(" << neg << ") = " << abs_val << " (expected: 5.5)" << std::endl;
    
    BFloat16 four(4.0f);
    BFloat16 sqrt_val = four.sqrt();
    std::cout << "sqrt(" << four << ") = " << sqrt_val << " (expected: 2.0)" << std::endl;
    
    BFloat16 nine(9.0f);
    std::cout << "sqrt(" << nine << ") = " << nine.sqrt() << " (expected: 3.0)" << std::endl;
    
    // test sqrt of negative (should return NaN)
    BFloat16 sqrt_neg = neg.sqrt();
    std::cout << "sqrt(" << neg << ") = " << sqrt_neg << " (expected: nan)" << std::endl;
    std::cout << "Is NaN: " << sqrt_neg.isNaN() << std::endl;
}

void testEdgeCases() {
    std::cout << "\nTesting Edge Cases" << std::endl;
    
    // x / 0
    BFloat16 one(1.0f);
    BFloat16 zero(0.0f);
    BFloat16 result = one / zero;
    std::cout << "1.0 / 0.0 = " << result << " (is infinity: " << result.isInfinity() << ")" << std::endl;
    
    // 0 / 0
    result = zero / zero;
    std::cout << "0.0 / 0.0 = " << result << " (is NaN: " << result.isNaN() << ")" << std::endl;
    
    // infinity arithmetic
    BFloat16 inf = BFloat16::infinity(false);
    result = inf + one;
    std::cout << "inf + 1.0 = " << result << " (is infinity: " << result.isInfinity() << ")" << std::endl;
    
    result = inf - inf;
    std::cout << "inf - inf = " << result << " (is NaN: " << result.isNaN() << ")" << std::endl;
    
    // subnormal numbers
    BFloat16 tiny = BFloat16::fromBits(0x0001); // Smallest positive subnormal
    std::cout << "\nSmallest positive subnormal:" << std::endl;
    std::cout << "Value: " << tiny.toDouble() << std::endl;
    std::cout << "Is subnormal: " << tiny.isSubnormal() << std::endl;
    tiny.printDetails();
}

void testBitRepresentation() {
    std::cout << "\nTesting Bit Representation" << std::endl;
    
    BFloat16 values[] = {
        BFloat16(0.0f),
        BFloat16(1.0f),
        BFloat16(-1.0f),
        BFloat16(2.0f),
        BFloat16(0.5f),
        BFloat16(3.14159f)
    };
    
    std::cout << std::fixed << std::setprecision(4);
    for (const auto& val : values) {
        std::cout << "\nValue: " << val << std::endl;
        std::cout << "Binary: " << val.toBinary() << std::endl;
        std::cout << "Hex: " << val.toHex() << std::endl;
    }
}

void testPrecisionLoss() {
    std::cout << "\nTesting Precision Loss" << std::endl;
    
    // large + small
    BFloat16 large(1e20f);
    BFloat16 small(1.0f);
    BFloat16 result = large + small;
    
    std::cout << std::scientific << std::setprecision(4);
    std::cout << "Large + small: " << large << " + " << small << " = " << result << std::endl;
    std::cout << "Lost precision: " << (result == large) << std::endl;
    
    std::cout << "\nNote: BFloat16 has only ~3 decimal digits of precision!" << std::endl;
    std::cout << "This is much less than FP32 (~7 digits) or FP64 (~15 digits)" << std::endl;
}

void testFP32Conversion() {
    std::cout << "\nTesting FP32 <-> BFloat16 Conversion" << std::endl;
    
    float original = 3.14159f;
    BFloat16 bf(original);
    float converted = bf.toFloat();
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Original FP32: " << original << std::endl;
    std::cout << "BFloat16: " << bf << std::endl;
    std::cout << "Converted back: " << converted << std::endl;
    std::cout << "Precision lost: " << (original - converted) << std::endl;
    
    // show bit pattern
    uint32_t fp32_bits;
    std::memcpy(&fp32_bits, &original, sizeof(float));
    
    std::cout << "\nBit patterns:" << std::endl;
    std::cout << "FP32 bits: 0x" << std::hex << std::uppercase << std::setw(8) 
              << std::setfill('0') << fp32_bits << std::endl;
    std::cout << "BFloat16:  " << bf.toHex() << " (upper 16 bits of FP32)" << std::endl;
}

void testDynamicRange() {
    std::cout << "\nTesting Dynamic Range" << std::endl;
    
    // bf16 exp range is same as fp32
    BFloat16 very_large(3.4e38f);
    BFloat16 very_small(1.2e-38f);
    
    std::cout << std::scientific << std::setprecision(2);
    std::cout << "Very large: " << very_large << std::endl;
    std::cout << "Very small: " << very_small << std::endl;
    
    std::cout << "\nBFloat16 advantage: Same dynamic range as FP32" << std::endl;
    std::cout << "BFloat16 limitation: Only 7 mantissa bits (vs 23 in FP32)" << std::endl;
    std::cout << "This makes it ideal for ML where range > precision" << std::endl;
}

int main() {
    std::cout << "   BFloat16 Educational Demo" << std::endl;
    
    testConstruction();
    testSpecialValues();
    testArithmetic();
    testComparison();
    testMathFunctions();
    testEdgeCases();
    testBitRepresentation();
    testPrecisionLoss();
    testFP32Conversion();
    testDynamicRange();
    
    std::cout << "   All tests completed!" << std::endl;    
    return 0;
}