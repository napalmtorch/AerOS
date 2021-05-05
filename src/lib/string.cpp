#include "lib/string.hpp"

extern "C"
{   
    // get length of string
    size_t strlen(char text[])
    {
        size_t i = 0;
        while (text[i] != '\0') { i++; } return i;
    }

    // get length of constant string
    size_t strlen_c(const char text[])
    {
        size_t i = 0;
        while (text[i] != '\0') { i++; } return i;
    }

    // convert string to decimal
    uint32_t stod(char text[])
    {
        uint32_t res = 0;
        for (size_t i = 0; text[i] != '\0'; i++) { res = res * 10 + text[i] - '0'; }
        return res;
    }

    // convert decimal to string
    void strdec(int32_t num, char text[])
    {
        int i, sign;
        if ((sign = num) < 0) num = -num;
        i = 0;
        do { text[i++] = num % 10 + '0'; } 
        while ((num /= 10) > 0);

        if (sign < 0) text[i++] = '-';
        text[i] = '\0';

        strrev(text);
    }

    const char hexVals[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    void strhex(uint32_t num, char text[])
    {
        text[0] = '\0';
        uint32_t bytes = 0;
        if (num <= 255) { bytes = 1; }
        else if (num > 255 && num <= 65535) { bytes = 2; }
        else if (num > 65535) { bytes = 4; }

        if (bytes == 1)
        {
            stradd(text, hexVals[(uint8_t)((num & 0xF0) >> 4)]);
            stradd(text, hexVals[(uint8_t)(num & 0x0F)]);
        }
        else if (bytes == 2)
        {
            stradd(text, hexVals[(uint8_t)((num & 0xF000) >> 12)]);
            stradd(text, hexVals[(uint8_t)((num & 0x0F00) >> 8)]);
            stradd(text, hexVals[(uint8_t)((num & 0x0F00) >> 4)]);
            stradd(text, hexVals[(uint8_t)(num & 0x000F)]);
        }
        else if (bytes == 4)
        {
            stradd(text, hexVals[(uint8_t)((num & 0xF0000000) >> 28)]);
            stradd(text, hexVals[(uint8_t)((num & 0x0F000000) >> 24)]);
            stradd(text, hexVals[(uint8_t)((num & 0x00F00000) >> 20)]);
            stradd(text, hexVals[(uint8_t)((num & 0x000F0000) >> 16)]);
            stradd(text, hexVals[(uint8_t)((num & 0x0000F000) >> 12)]);
            stradd(text, hexVals[(uint8_t)((num & 0x00000F00) >> 8)]);
            stradd(text, hexVals[(uint8_t)((num & 0x000000F0) >> 4)]);
            stradd(text, hexVals[(uint8_t)(num & 0x0000000F)]);
        }
    }

    // convert hexadecimal to string
    void strhex32(uint32_t num, char text[])
    {
        text[0] = '\0';
        stradd(text, hexVals[(uint8_t)((num & 0xF0000000) >> 28)]);
        stradd(text, hexVals[(uint8_t)((num & 0x0F000000) >> 24)]);
        stradd(text, hexVals[(uint8_t)((num & 0x00F00000) >> 20)]);
        stradd(text, hexVals[(uint8_t)((num & 0x000F0000) >> 16)]);
        stradd(text, hexVals[(uint8_t)((num & 0x0000F000) >> 12)]);
        stradd(text, hexVals[(uint8_t)((num & 0x00000F00) >> 8)]);
        stradd(text, hexVals[(uint8_t)((num & 0x000000F0) >> 4)]);
        stradd(text, hexVals[(uint8_t)(num & 0x0000000F)]);
    }

    // reverse string
    void strrev(char text[]) 
    {
        int c, i, j;
        for (i = 0, j = strlen(text)-1; i < j; i++, j--) 
        {
            c = text[i];
            text[i] = text[j];
            text[j] = c;
        }
    }

    // add character to string
    void stradd(char text[], char c)
    {
        size_t len = strlen(text);
        text[len] = c;
        text[len + 1] = '\0';
    }

    // add string to string
    void strcat(char text[], char to_add[])
    {
        for (size_t i = 0; i < strlen(to_add); i++) { stradd(text, to_add[i]); }
    }
    // delete last character of string
    void strdel(char text[])
    {
        size_t len = strlen(text);
        text[len - 1] = '\0';
    }

    // convert string to all upper case
    void strupper(char text[])
    {
        for (size_t i = 0; i < strlen(text); i++)
        {
            if (text[i] >= 97 && text[i] <= 122) { text[i] -= 32; }
        }
    }

    // convert string to all lower case
    void strlower(char text[])
    {
        for (size_t i = 0; i < strlen(text); i++)
        {
            if (text[i] >= 65 && text[i] <= 90) { text[i] += 32; }
        }
    }

    // copy string
    char* strcpy(char* src, char* dest)
    {
        if (dest == nullptr) { return nullptr; }
        char* ptr = dest;
        while (*src != '\0')
        {
            *dest = *src;
            dest++; src++;
        }
        *dest = '\0';
        return ptr;
    }

    // copy constant string
    char* strcpy_c(const char src[], char* dest)
    {
        if (dest == nullptr) { return nullptr; }
        char* ptr = dest;
        while (*src != '\0')
        {
            *dest = *src;
            dest++; src++;
        }
        *dest = '\0';
        return ptr;
    }

    // compare strings
    int32_t strcmp(char text[], char comp[])
    {
        size_t i;
        for (i = 0; text[i] == comp[i]; i++) { if (text[i] == '\0') { return 0; } }
        return text[i] - comp[i];
    }

    // check if string equal to string
    bool streql(char text[], char comp[]) { return strcmp(text, comp) == 0; }
}