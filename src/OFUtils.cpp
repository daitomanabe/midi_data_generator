//
//  OFUtils.cpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/07/24.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#include "OFUtils.hpp"
using namespace std;


string ofGetTimestampString(){

    string timeFormat = "%Y-%m-%d-%H-%M-%S-%i";

    return ofGetTimestampString(timeFormat);
}
void ofStringReplace(string& input, const string& searchStr, const string& replaceStr){
    auto pos = input.find(searchStr);
    while(pos != std::string::npos){
        input.replace(pos, searchStr.size(), replaceStr);
        pos += replaceStr.size();
        std::string nextfind(input.begin() + pos, input.end());
        auto nextpos = nextfind.find(searchStr);
        if(nextpos==std::string::npos){
            break;
        }
        pos += nextpos;
    }
}

//specify the string format - eg: %Y-%m-%d-%H-%M-%S-%i ( 2011-01-15-18-29-35-299 )
//--------------------------------------------------
string ofGetTimestampString(const string& timestampFormat){
    std::stringstream str;
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);    std::chrono::duration<double> s = now - std::chrono::system_clock::from_time_t(t);
    int ms = s.count() * 1000;
    auto tm = *std::localtime(&t);
    constexpr int bufsize = 256;
    char buf[bufsize];

    // Beware! an invalid timestamp string crashes windows apps.
    // so we have to filter out %i (which is not supported by vs)
    // earlier.
    auto tmpTimestampFormat = timestampFormat;
    ofStringReplace(tmpTimestampFormat, "%i", ofToString(ms, 3, '0'));

    if (strftime(buf,bufsize, tmpTimestampFormat.c_str(),&tm) != 0){
        str << buf;
    }
    auto ret = str.str();


    return ret;
}
