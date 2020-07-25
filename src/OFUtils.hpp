//
//  OFUtils.hpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/07/24.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef OFUtils_hpp
#define OFUtils_hpp

#include <stdio.h>
//#include <bitset> // For ofToBinary.
#include <chrono>
#include <iomanip>  //for setprecision
#include <algorithm>
#include <sstream>


template <class T>
std::string ofToString(const T& value){
    std::ostringstream out;
    out << value;
    return out.str();
}

template <class T>
std::string ofToString(const T& value, int precision){
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

/// \brief Convert a value to a string with a specific width and fill
///
/// Like sprintf "% 4d" or "% 4f" format, in this example width=4, fill=' '
///
/// \tparam T The data type of the value to convert to a string.
/// \param value The value to convert to a string.
/// \param width The width of the value to use when converting to a string.
/// \param fill The character to use when padding the converted string.
/// \returns The string representation of the value.
template <class T>
std::string ofToString(const T& value, int width, char fill ){
    std::ostringstream out;
    out << std::fixed << std::setfill(fill) << std::setw(width) << value;
    return out.str();
}

/// \brief Convert a value to a string with a specific precision, width and filll
///
/// Like sprintf "%04.2d" or "%04.2f" format, in this example precision=2, width=4, fill='0'
///
/// \tparam T The data type of the value to convert to a string.
/// \param value The value to convert to a string.
/// \param precision The precision to use when converting to a string.
/// \param width The width of the value to use when converting to a string.
/// \param fill The character to use when padding the converted string.
/// \returns The string representation of the value.
template <class T>
std::string ofToString(const T& value, int precision, int width, char fill ){
    std::ostringstream out;
    out << std::fixed << std::setfill(fill) << std::setw(width) << std::setprecision(precision) << value;
    return out.str();
}

/// \brief Convert a vector of values to a comma-delimited string.
///
/// This method will take any vector of values and output a list of the values
/// as a comma-delimited string.
///
/// \tparam T The data type held by the vector.
/// \param values The vector of values to be converted to a string.
/// \returns a comma-delimited string representation of the intput values.
template<class T>
std::string ofToString(const std::vector<T>& values) {
    std::stringstream out;
    int n = values.size();
    out << "{";
    if(n > 0) {
        for(int i = 0; i < n - 1; i++) {
            out << values[i] << ", ";
        }
        out << values[n - 1];
    }
    out << "}";
    return out.str();
}

void ofStringReplace(std::string& input, const std::string& searchStr, const std::string& replaceStr);


std::string ofGetTimestampString();

std::string ofGetTimestampString(const std::string& timestampFormat);

#endif /* OFUtils_hpp */
