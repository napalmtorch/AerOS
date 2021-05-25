#pragma once
#include "lib/types.h"

#ifdef __cplusplus
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
        char* strrev(char text[]);
        // add character to string
        void stradd(char text[], char c);
        // add string to string
        void strcat(char text[], char to_add[]);
        // delete last character of string
        void strdel(char text[]);
        // substring
        char* strsub(char* str, uint16_t start, uint16_t end);
        // convert string to all upper case
        void strupper(char text[]);
        // convert string to all lower case
        char* strlower(char text[]);
        // copy string
        char* strcpy(char* src, char* dest);
        //check if string contains
        char * strstr(char * haystack, char * needle);
        bool StringContains(char* text, char* find);
        // copy constant string
        char* strcpy_c(const char src[], char* dest);
        //check if string starts with a specific word
        uint8_t startswith(const char * str, const char * accept);
        // compare strings
        int32_t strcmp(char text[], char comp[]);
        // check if string equal to string
        bool streql(char text[], char comp[]);
        char* strsplit_index(char text[], int index, char separator);
        char** strsplit(char text[], char separator, uint32_t *len);
        bool IsEmpty(char* text);
        int strindexof(char text[], char character);
        char * stringReplace(char *search, char *replace, char *string);
    }

namespace System
{
    class String
    {
        private:
            char* Data;
        public:
            // default constructor
            String();
            // constructor with pointer
            String(char* text);
            // constructor with two pointers
            String(char* left, char* right);
            // constructor copy
            String(const String& text);
            // constructor move
            String(String&& text);
            // destructor
            ~String();
            // set text
            void Set(const char text[]);
            // append character
            String& Append(char c);
            // append constant string
            String& Append(const char text[]);
            // remove last
            String& RemoveLast();
            // substring
            char* Substring(uint32_t index, uint32_t len);
            // get character at index
            char GetChar(uint32_t index);
            // get index of first instance of character
            uint32_t IndexOf(char c);
            // get index of last instance of character
            uint32_t LastIndexOf(char c);
            // convert to upper case
            void ToUpper();
            // compare two strings
            uint32_t CompareTo(String& text);
            // is string equal to another string
            bool Equals(String& text);
            // is string equal to another constant string
            bool Equals(char text[]);
            // convert string to integer
            uint32_t ToDecimal();
            // get length of string
            size_t GetLength();
            // return character array pointer
            char* ToCharArray();

            // assign operators
            String & operator = (const char []);
            String & operator = (const String &);
            String & operator = (String &&);

            // add-assign operators
            String & operator += (const char []);
            String & operator += (const String &);
            String & operator += (String &&);

            // equals operators
            bool operator == (char []);
            bool operator == (String);
    };
}
#else
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
    // substring
    char* strsub(char* str, uint16_t start, uint16_t end);
    // convert string to all upper case
    void strupper(char text[]);
    // convert string to all lower case
    void strlower(char text[]);
    // copy string
    char* strcpy(char* src, char* dest);
    // copy constant string
    char* strcpy_c(const char src[], char* dest);
    char * strstr(char * haystack, char * needle);
    bool StringContains(char* text, char* find);
    // compare strings
    int32_t strcmp(char text[], char comp[]);
    // check if string equal to string
    bool streql(char text[], char comp[]);
    char* strsplit_index(char text[], int index, char separator);
    bool IsEmpty(char* text);
#endif