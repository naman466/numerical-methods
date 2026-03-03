#include "FP32.h"
#include <iostream>
#include <iomanip>
#include <cassert>

void testConstruction() {
    std::cout << "\nConstruction" << std::endl;
    
    FP32 zero;
    std::cout << "Default (zero): " << zero << std::endl;
    zero.printDetails();
    
    FP32 one(1.0f);
    std::cout << "\nFP32(1.0f): " << one << std::endl;
    one.printDetails();
    
    FP32 neg_half(-0.5f);
    std::cout << "\nFP32(-0.5f): " << neg_half << std::endl;
    neg_half.printDetails();
    
    FP32 from_bits = FP32::fromBits(0x3F800000); // 1.0
    std::cout << "\nFrom bits 0x3F800000: " << from_bits << std::endl;
}

void testSpecialValues() {
    std::cout << "\nSpecial Values" << std::endl;
    
    FP32 pos_inf = FP32::infinity(false);
    FP32 neg_inf = FP32::infinity(true);
    FP32 nan = FP32::nan();
    FP32 pos_zero = FP32::zero(false);
    FP32 neg_zero = FP32::zero(true);
    FP32 epsilon = FP32::epsilon();
    
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
    std::cout << "FP32(1.0).isNormal(): " << FP32(1.0f).isNormal() << std::endl;
}

void testArithmetic() {
    std::cout << "\nArithmetic" << std::endl;
    
    FP32 a(2.5f);
    FP32 b(1.5f);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    
    FP32 sum = a + b;
    std::cout << "a + b = " << sum << " (expected: 4.0)" << std::endl;
    
    FP32 diff = a - b;
    std::cout << "a - b = " << diff << " (expected: 1.0)" << std::endl;
    
    FP32 prod = a * b;
    std::cout << "a * b = " << prod << " (expected: 3.75)" << std::endl;
    
    FP32 quot = a / b;
    std::cout << "a / b = " << quot << " (expected: 1.666667)" << std::endl;
    
    FP32 neg_a = -a;
    std::cout << "-a = " << neg_a << " (expected: -2.5)" << std::endl;
    
    // test with larger values
    std::cout << "\nLarger value tests:" << std::endl;
    FP32 large1(1000000.0f);
    FP32 large2(2000000.0f);
    std::cout << large1 << " + " << large2 << " = " << (large1 + large2) << std::endl;
    std::cout << large2 << " - " << large1 << " = " << (large2 - large1) << std::endl;
}

void testComparison() {
    std::cout << "\nComparison" << std::endl;
    
    FP32 a(2.0f);
    FP32 b(3.0f);
    FP32 c(2.0f);
    
    std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
    std::cout << "a == c: " << (a == c) << " (expected: 1)" << std::endl;
    std::cout << "a != b: " << (a != b) << " (expected: 1)" << std::endl;
    std::cout << "a < b: " << (a < b) << " (expected: 1)" << std::endl;
    std::cout << "b > a: " << (b > a) << " (expected: 1)" << std::endl;
    std::cout << "a <= c: " << (a <= c) << " (expected: 1)" << std::endl;
    std::cout << "a >= c: " << (a >= c) << " (expected: 1)" << std::endl;
    
    // test with negative numbers
    FP32 neg(-5.0f);
    FP32 pos(5.0f);
    std::cout << "\nneg = " << neg << ", pos = " << pos << std::endl;
    std::cout << "neg < pos: " << (neg < pos) << " (expected: 1)" << std::endl;
    
    // test zero equality
    FP32 pos_zero = FP32::zero(false);
    FP32 neg_zero = FP32::zero(true);
    std::cout << "\npos_zero == neg_zero: " << (pos_zero == neg_zero) << " (expected: 1)" << std::endl;
}

void testMathFunctions() {
    std::cout << "\nMath Functions" << std::endl;
    
    FP32 neg(-5.5f);
    FP32 abs_val = neg.abs();
    std::cout << "abs(" << neg << ") = " << abs_val << " (expected: 5.5)" << std::endl;
    
    FP32 four(4.0f);
    FP32 sqrt_val = four.sqrt();
    std::cout << "sqrt(" << four << ") = " << sqrt_val << " (expected: 2.0)" << std::endl;
    
    FP32 nine(9.0f);
    std::cout << "sqrt(" << nine << ") = " << nine.sqrt() << " (expected: 3.0)" << std::endl;
    
    // test sqrt of negative (should return NaN)
    FP32 sqrt_neg = neg.sqrt();
    std::cout << "sqrt(" << neg << ") = " << sqrt_neg << " (expected: nan)" << std::endl;
    std::cout << "Is NaN: " << sqrt_neg.isNaN() << std::endl;
}

void testEdgeCases() {
    std::cout << "\nEdge Cases" << std::endl;
    
    // division by zero
    FP32 one(1.0f);
    FP32 zero(0.0f);
    FP32 result = one / zero;
    std::cout << "1.0 / 0.0 = " << result << " (is infinity: " << result.isInfinity() << ")" << std::endl;
    
    // zero divided by zero
    result = zero / zero;
    std::cout << "0.0 / 0.0 = " << result << " (is NaN: " << result.isNaN() << ")" << std::endl;
    
    // infinity arithmetic
    FP32 inf = FP32::infinity(false);
    result = inf + one;
    std::cout << "inf + 1.0 = " << result << " (is infinity: " << result.isInfinity() << ")" << std::endl;
    
    result = inf - inf;
    std::cout << "inf - inf = " << result << " (is NaN: " << result.isNaN() << ")" << std::endl;
    
    // very small numbers (subnormal)
    FP32 tiny = FP32::fromBits(0x00000001); // Smallest positive subnormal
    std::cout << "\nSmallest positive subnormal:" << std::endl;
    std::cout << "Value: " << tiny.toDouble() << std::endl;
    std::cout << "Is subnormal: " << tiny.isSubnormal() << std::endl;
    tiny.printDetails();
}

void testBitRepresentation() {
    std::cout << "\nBit Representation" << std::endl;
    
    FP32 values[] = {
        FP32(0.0f),
        FP32(1.0f),
        FP32(-1.0f),
        FP32(2.0f),
        FP32(0.5f),
        FP32(3.14159f)
    };
    
    std::cout << std::fixed << std::setprecision(6);
    for (const auto& val : values) {
        std::cout << "\nValue: " << val << std::endl;
        std::cout << "Binary: " << val.toBinary() << std::endl;
        std::cout << "Hex: " << val.toHex() << std::endl;
    }
}

void testPrecisionLoss() {
    std::cout << "\nPrecision Los" << std::endl;
    
    // large + small number
    FP32 large(1e20f);
    FP32 small(1.0f);
    FP32 result = large + small;
    
    std::cout << std::scientific << std::setprecision(6);
    std::cout << "Large + small: " << large << " + " << small << " = " << result << std::endl;
    std::cout << "Lost precision: " << (result == large) << std::endl;
    
    // catastrophic cancellation
    FP32 a(1.0000001f);
    FP32 b(1.0f);
    result = a - b;
    std::cout << "\nCatastrophic cancellation:" << std::endl;
    std::cout << a << " - " << b << " = " << result << std::endl;
}

int main() {
    
    testConstruction();
    testSpecialValues();
    testArithmetic();
    testComparison();
    testMathFunctions();
    testEdgeCases();
    testBitRepresentation();
    testPrecisionLoss();
    
    std::cout << " All tests completed!" << std::endl;
    
    return 0;
}