#include "FP32.h"
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, const FP32& fp) {
    // save stream state
    std::ios_base::fmtflags flags = os.flags();
    std::streamsize precision = os.precision();
    
    // print the decimal value (with approx precision)
    os << std::setprecision(9) << fp.toDouble();
    
    // restore stream state
    os.flags(flags);
    os.precision(precision);
    
    return os;
}

std::istream& operator>>(std::istream& is, FP32& fp) {
    float value;
    is >> value;
    fp = FP32(value);
    return is;
}