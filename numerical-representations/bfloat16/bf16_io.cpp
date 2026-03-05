#include "BF16.h"
#include <iostream>
#include <iomanip>


std::ostream& operator<<(std::ostream& os, const BFloat16& bf) {
    // save stream state
    std::ios_base::fmtflags flags = os.flags();
    std::streamsize precision = os.precision();
    
    // print the decimal value with appropriate precision 
    os << std::setprecision(4) << bf.toDouble();
    
    // restore stream state
    os.flags(flags);
    os.precision(precision);
    
    return os;
}

std::istream& operator>>(std::istream& is, BFloat16& bf) {
    float value;
    is >> value;
    bf = BFloat16(value);
    return is;
}