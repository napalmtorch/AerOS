#pragma once
#include "lib/types.h"

extern "C"
{
    // get length of string
    size_t strlen(char text[]);
    // get length of constant string
    size_t strlen_c(const char text[]);
    // convert string to decimal
    uint32_t stod(char text[]);
    // convert decimal to string
    void strdec(int32_t num, char text[]);
    // convert hexadecimal to string
    void strhex(uint32_t num, char text[]);
    // convert hexadecimal to string
    void strhex32(uint32_t num, char text[]);
    // reverse string
    void strrev(char text[]);
    // add character to string
    void stradd(char text[], char c);
    // add string to string
    void strcat(char text[], char to_add[]);
    // delete last character of string
    void strdel(char text[]);
    // split string
    void strsplit(char text[], char* dest[], size_t* lenght);
    // convert string to all upper case
    void strupper(char text[]);
    // convert string to all lower case
    void strlower(char text[]);
    // copy string
    char* strcpy(char* src, char* dest);
    // copy constant string
    char* strcpy_c(const char src[], char* dest);
    // compare strings
    int32_t strcmp(char text[], char comp[]);
    // check if string equal to string
    bool streql(char text[], char comp[]);
    char* strsplit_index(char text[], int index, char separator);
    bool IsEmpty(char* text);
}

