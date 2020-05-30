#include <string>

#ifndef LIB_UTILS_H_
#define LIB_UTILS_H_

char* str2char(std::string s){
    char * ch = new char [s.length()+1];
    strcpy (ch, s.c_str());
    return ch;
}

#endif