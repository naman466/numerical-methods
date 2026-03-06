#include "BF16.h"
#include <iostream>
#include <iomanip>
#include <cstring>

int main() {
    std::cout << "BFloat16 Demo" << std::endl;
    std::cout << std::endl;
    
    std::cout << "BFloat16 representation" << std::endl;
    BFloat16 pi(3.14159f);
    std::cout << "Creating BFloat16 for π ≈ 3.14159" << std::endl;
    pi.printDetails();
    
    std::cout << "\nComparing with FP32:" << std::endl;
    float pi_fp32 = 3.14159f;
    std::cout << "FP32 value: " << std::setprecision(8) << pi_fp32 << std::endl;
    std::cout << "BFloat16 value: " << pi.toFloat() << std::endl;
    std::cout << "Precision loss: " << (pi_fp32 - pi.toFloat()) << std::endl;
    
    std::cout << "\nBasic Arithmetic" << std::endl;
    BFloat16 a(2.5f);
    BFloat16 b(1.5f);
    
    std::cout << std::setprecision(4);
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "a + b = " << (a + b) << std::endl;
    std::cout << "a - b = " << (a - b) << std::endl;
    std::cout << "a * b = " << (a * b) << std::endl;
    std::cout << "a / b = " << (a / b) << std::endl;
    
    std::cout << "\nDynamic Range" << std::endl;
    
    BFloat16 huge(1e38f);
    BFloat16 tiny(1e-38f);
    
    std::cout << std::scientific << std::setprecision(2);
    std::cout << "\nVery large number: " << huge << std::endl;
    std::cout << "Very small number: " << tiny << std::endl;
    
    std::cout << "\nPrecision Limitations" << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    
    float fp32_val = 1.234567f;
    BFloat16 bf16_val(fp32_val);
    
    std::cout << "FP32:    " << fp32_val << " (23 mantissa bits, ~7 decimal digits)" << std::endl;
    std::cout << "BFloat16: " << bf16_val.toFloat() << " (7 mantissa bits, ~2-3 decimal digits)" << std::endl;

    
    std::cout << "\nFP32 <-> BFloat16" << std::endl;
    
    float original = 100.5f;
    BFloat16 bf(original);
    
    uint32_t fp32_bits;
    std::memcpy(&fp32_bits, &original, sizeof(float));
    
    std::cout << std::hex << std::uppercase;
    std::cout << "\nFP32 bits:   0x" << std::setw(8) << std::setfill('0') << fp32_bits << std::endl;
    std::cout << "BFloat16:    " << bf.toHex() << std::endl;
    std::cout << "Expanded:    0x" << std::setw(8) << std::setfill('0') << bf.toFP32Bits() << std::endl;
    
    std::cout << std::dec;
    
    std::cout << "\nComparing Bit Patterns" << std::endl;
    BFloat16 values[] = {BFloat16(1.0f), BFloat16(2.0f), BFloat16(0.5f)};
    std::string names[] = {"1.0", "2.0", "0.5"};
    
    std::cout << std::left;
    std::cout << "\nBFloat16 (16-bit):" << std::endl;
    for (size_t i = 0; i < 3; ++i) {
        std::cout << std::setw(6) << names[i] << " : " 
                  << values[i].toBinary() << " : " 
                  << values[i].toHex() << std::endl;
    }
    
    std::cout << "\nSpecial Values" << std::endl;
    BFloat16 inf = BFloat16::infinity(false);
    BFloat16 nan = BFloat16::nan();
    BFloat16 zero = BFloat16::zero();
    
    std::cout << "Infinity: " << inf.toHex() << std::endl;
    std::cout << "NaN:      " << nan.toHex() << std::endl;
    std::cout << "Zero:     " << zero.toHex() << std::endl;
    
    return 0;
}