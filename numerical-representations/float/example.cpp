#include "FP32.h"
#include <iostream>
#include <iomanip>

int main() {    
    // basic construction
    std::cout << "--- Example 1 ---" << std::endl;
    FP32 pi(3.14159f);
    std::cout << "Creating FP32 for π ≈ 3.14159" << std::endl;
    pi.printDetails();
    
    // arithmetic
    std::cout << "\n--- Example 2 ---" << std::endl;
    FP32 a(2.5f);
    FP32 b(1.5f);
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "a + b = " << (a + b) << std::endl;
    std::cout << "a - b = " << (a - b) << std::endl;
    std::cout << "a * b = " << (a * b) << std::endl;
    std::cout << "a / b = " << (a / b) << std::endl;
    
    // special values
    std::cout << "\n--- Example 3 ---" << std::endl;
    FP32 inf = FP32::infinity(false);
    FP32 nan = FP32::nan();
    FP32 zero = FP32::zero();
    
    std::cout << "Positive Infinity: " << inf << " (hex: " << inf.toHex() << ")" << std::endl;
    std::cout << "NaN: " << nan << " (hex: " << nan.toHex() << ")" << std::endl;
    std::cout << "Zero: " << zero << " (hex: " << zero.toHex() << ")" << std::endl;
    
    // edge cases
    std::cout << "\n--- Example ---" << std::endl;
    FP32 one(1.0f);
    FP32 result = one / zero;
    std::cout << "1.0 / 0.0 = " << result;
    std::cout << " (isInfinity: " << result.isInfinity() << ")" << std::endl;
    
    result = zero / zero;
    std::cout << "0.0 / 0.0 = " << result;
    std::cout << " (isNaN: " << result.isNaN() << ")" << std::endl;
    
    // bit patterns
    std::cout << "\n--- Example 5 ---" << std::endl;
    FP32 values[] = {FP32(1.0f), FP32(2.0f), FP32(0.5f), FP32(-1.0f)};
    std::string names[] = {"1.0", "2.0", "0.5", "-1.0"};
    
    std::cout << std::left;
    for (size_t i = 0; i < 4; ++i) {
        std::cout << std::setw(6) << names[i] << " : " 
                  << values[i].toBinary() << " : " 
                  << values[i].toHex() << std::endl;
    }
    
    // precision limits
    std::cout << "\n--- Example 6 ---" << std::endl;
    FP32 large(16777216.0f);  // 2^24
    FP32 small(1.0f);
    FP32 sum = large + small;
    
    std::cout << "Large number: " << large << std::endl;
    std::cout << "Small number: " << small << std::endl;
    std::cout << "Large + Small = " << sum << std::endl;
    
    // subnormal numbers
    std::cout << "\n--- Example 7 ---" << std::endl;
    FP32 tiny = FP32::fromBits(0x00000001);  // smallest pos subnormal
    FP32 small_normal = FP32::fromBits(0x00800000);  // smallest normal
    
    std::cout << "Smallest subnormal number:" << std::endl;
    std::cout << "  Value: " << tiny.toDouble() << std::endl;
    std::cout << "  Binary: " << tiny.toBinary() << std::endl;
    std::cout << "  Is subnormal: " << tiny.isSubnormal() << std::endl;
    
    std::cout << "  Value: " << small_normal.toDouble() << std::endl;
    std::cout << "  Binary: " << small_normal.toBinary() << std::endl;
    std::cout << "  Is normal: " << small_normal.isNormal() << std::endl;
    
    return 0;
}
