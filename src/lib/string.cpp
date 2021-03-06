#include "lib/string.hpp"
#include "core/kernel.hpp"
#include "lib/list.hpp"

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
uint32_t stod(char text[]) //ok try to build
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
        stradd(text, hexVals[(uint8_t)((num & 0x00F0) >> 4)]);
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
char* strrev(char text[]) 
{
    int c, i, j;
    for (i = 0, j = strlen(text)-1; i < j; i++, j--) 
    {
        c = text[i];
        text[i] = text[j];
        text[j] = c;
    }
    return text;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned int count = 0;
	while (count < n)
	{
		if (s1[count] == s2[count])
		{
			if (s1[count] == '\0') //quit early because null-termination found
				return 0;
			else
				count++;
		}
		else
			return s1[count] - s2[count];
	}
	
	return 0;
}

int EndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen_c(str);
    size_t lensuffix = strlen_c(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
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

// substring
char* strsub(char* str, uint16_t start, uint16_t end)
{
    if (strlen(str) < start) return nullptr;
    char* ret = (char* )mem_alloc((sizeof(char)) * (end - start + 1));
    ret[end-start] = 0;

    for(uint16_t i = 0; i < (end - start); i++) { ret[i] = str[i + start]; }
    return ret;
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
char* strlower(char text[])
{
    for (size_t i = 0; i < strlen(text); i++)
    {
        if (text[i] >= 65 && text[i] <= 90) { text[i] += 32; }
    }
    return text;
}
//check if string is empty or not
bool IsEmpty(char* text)
{
    size_t len = strlen(text);
    if(len == 0 || len < 0 || text == nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
} 
bool StringContains(char* text, char* find)
{
    if (strstr(text,find) != nullptr)
    {
        return true;
    }
    return false;
}
int strindexof(char text[], char character)
{
    for (int i = 0; text[i] != 0; i++)
    {
        if (text[i] == character)
        {
            return i;
        }
    }
    return -1;
}

// "hello world" strsplit_index(' ', 1) = "world"
char* strsplit_index(char text[], int index, char separator)
{
    size_t len = strlen(text);
    //man no this has to start from 0
    int last = -1;
    for (int i = 0, ind = 0; i < len; i += strindexof(text+i, separator) + 1, ind++)
    {
        if (i == last)
            break;
        if (index == ind)
        {
            int len = strindexof(text+i, separator);
            if (len < 0)
                len = strlen(text+i);
            char* str = new char[len + 1];
            for (int xi = 0; xi < len; xi++)
                str[xi] = (text+i)[xi];
            str[len] = 0;
            return str; // try now
        }
        last = i;
    }
    
    return nullptr;
}
char** strsplit(char text[], char separator, uint32_t* len) //brb, i leave this open, code as you wish - try this - how do i use it
{
    System::List<char*> *data = new System::List<char*>;
    for (; strsplit_index(text, *len, separator) != nullptr;)
    {
        data->Add(strsplit_index(text, *len, separator));
        *len +=1;
        continue;
    }
    return data->ToArray();
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

char * strstr(char * haystack, char * needle) {
	const char * out = NULL;
	const char * ptr;
	const char * acc;
	const char * p;
	size_t s = strlen_c(needle);
	for (ptr = haystack; *ptr != '\0'; ++ptr) {
		size_t accept = 0;
		out = ptr;
		p = ptr;
		for (acc = needle; (*acc != '\0') && (*p != '\0'); ++acc) {
			if (*p == *acc) {
				accept++;
				p++;
			} else {
				break;
			}
		}
		if (accept == s) {
			return (char *)out;
		}
	}
	return NULL;
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
//check if string starts with a specific word
uint8_t startswith(const char * str, const char * accept) {
	size_t s = strlen_c(accept);
	for (size_t i = 0; i < s; ++i) {
		if (*str != *accept) return 0;
		str++;
		accept++;
	}
	return 1;
}
// compare strings
int32_t strcmp(char text[], char comp[])
{
    size_t i;
    for (i = 0; text[i] == comp[i]; i++) { if (text[i] == '\0') { return 0; } }
    return text[i] - comp[i];
}

// check if string equal to string
bool streql(char text[], char comp[]) { return (bool)strcmp(text, comp) == 0; }

char * stringReplace(char *search, char *replace, char *strings) {
	char *tempString, *searchStart;
	int len=0;


	// check if the search string exists
	searchStart = strstr(strings, search);
	if(searchStart == NULL) {
		return strings;
	}

	// Lets reserve some memory
	tempString = (char*) mem_alloc(strlen(strings) * sizeof(char));
	if(tempString == NULL) {
        debug_writeln("String is empty uh oh");
		return NULL;
	}

	// Create temporary copy
	strcpy(strings,tempString);
	// set first segment in string
	len = searchStart - strings;
	strings[len] = '\0';

	// append second segment
	strcat(strings, replace);

	// finally append third segment
	len += strlen(search);
	strcat(strings, (char*)tempString+len);

	// release memory
	mem_free(tempString);
	//return the replaced string
	return strings;
}

namespace System
{
    // default constructor
    String::String() { Set(""); }

    // constructor with pointer
    String::String(char* text) { Set(text); }

    // constructor with two pointers
    String::String(char* left, char* right)
    {
        if (this->Data != nullptr) { delete this->Data; }
        uint32_t len = strlen(left) + strlen(right);
        this->Data = new char[len + 1];

        int pos = 0;
        for (size_t i = 0; i < strlen(left); i++) { this->Data[pos] = left[i]; pos++; }
        for (size_t i = 0; i < strlen(right); i++) { this->Data[pos] = right[i]; pos++; }
        ToCharArray()[len - 1] = '\0';
    }

    // constructor copy
    String::String(const String& text)
    {
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[strlen(text.Data) + 1];
        strcpy(text.Data, this->Data);
        ToCharArray()[strlen(text.Data)] = '\0';
    }

    // constructor move
    String::String(String&& text)
    {
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[strlen(text.Data) + 1];
        strcpy(text.Data, this->Data);
        ToCharArray()[strlen(text.Data)] = '\0';
        delete text.Data;
    }

    // destructor
    String::~String() { delete Data; Data = nullptr; }

    // set text
    void String::Set(const char text[])
    {
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[strlen_c(text) + 1];
        strcpy_c(text, this->Data);
        ToCharArray()[strlen_c(text)] = '\0';
    }
    
    // append character
    String& String::Append(char c)
    {
        uint32_t len = strlen(ToCharArray()) + 1;
        char temp[len];
        for (size_t i = 0; i < strlen(ToCharArray()); i++) { temp[i] = Data[i]; }
        temp[len - 1] = c;
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[len];
        strcpy(temp, ToCharArray());
        ToCharArray()[len - 1] = '\0';
        return *this;
    }

    // append constant string
    String& String::Append(const char text[])
    {
        uint32_t len = strlen(ToCharArray()) + strlen_c(text) + 1;
        char temp[len];
        uint32_t pos = 0;
        for (size_t i = 0; i < strlen(ToCharArray()); i++)
        {
            temp[pos] = ToCharArray()[i];
            pos++;
        }
        for (size_t i = 0; i < strlen_c(text); i++)
        {
            temp[pos] = text[i];
            pos++;
        }
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[len];
        strcpy(temp, ToCharArray());
        ToCharArray()[len - 1] = '\0';
        return *this;
    }

    // remove last
    String& String::RemoveLast()
    {
        uint32_t len = strlen(ToCharArray());
        char temp[len];
        uint32_t pos = 0;
        for (size_t i = 0; i < len; i++)
        {
            temp[pos] = ToCharArray()[i];
            pos++;
        }
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[len];
        strcpy(temp, ToCharArray());
        ToCharArray()[len - 1] = '\0';
        return *this;
    }

    // substring
    char* String::Substring(uint32_t index, uint32_t len)
    {
        char* temp = new char[len];
        for (size_t i = 0; i < len; i++)
        {
            temp[i] = Data[index + i];
        }
        return temp;
    }

    // get character at index
    char String::GetChar(uint32_t index)
    {
        if (index >= GetLength()) { return 0; }
        char data = Data[index];
        return data;
    }

    // get index of first instance of character
    uint32_t String::IndexOf(char c)
    {
        for (size_t i = 0; i < GetLength(); i++) { if (Data[i] == c) { return i; } }
        return 0;
    }

    // get index of last instance of character
    uint32_t String::LastIndexOf(char c)
    {
        for (size_t i = GetLength() - 1; i >= 0; i--) { if (Data[i] == c) { return i; } }
    }

     // convert to upper case
    void String::ToUpper()
    {
        for (size_t i = 0; i < GetLength(); i++)
        {
            if (Data[i] >= 97 && Data[i] <= 122) { Data[i] -= 32; }
        }
    }
    
    // compare two strings
    uint32_t String::CompareTo(String& text) { return strcmp(this->ToCharArray(), text.ToCharArray()); }

    // is string equal to another string
    bool String::Equals(String& text) { return strcmp(this->ToCharArray(), text.ToCharArray()) == 0; }

    // is string equal to another constant string
    bool String::Equals(char text[]) { return strcmp(this->ToCharArray(), text) == 0; }

    // convert string to integer
    uint32_t String::ToDecimal()
    {
        uint32_t output = stod(this->ToCharArray());
        return output;
    }

    // return length of string
    size_t String::GetLength() { return strlen(this->Data); }

    // return character array pointer
    char* String::ToCharArray() { return this->Data; }

    // assign operator
    String &String::operator = (const char text[])
    {
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[strlen_c(text) + 1];
        strcpy_c(text, this->Data);
        ToCharArray()[strlen_c(text)] = '\0';
        return *this;
    }
    String &String::operator = (const String &text)
    {
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[strlen(text.Data) + 1];
        strcpy(text.Data, this->Data);
        ToCharArray()[strlen(text.Data)] = '\0';
        return *this;
    }
    String &String::operator = (String &&text)
    {
        if (this->Data != nullptr) { delete this->Data; }
        this->Data = new char[strlen(text.Data) + 1];
        strcpy(text.Data, this->Data);
        ToCharArray()[strlen(text.Data)] = '\0';
        return *this;
    }

    // plus equal operator
    String &String::operator += (const char text[]) { Append(text); return *this; }
    String &String::operator += (const String &text) { Append(text.Data); return *this; }
    String &String::operator += (String &&text) { Append(text.Data); return *this; }

    bool String::operator == (char text[]) { return streql(this->Data, text); }
    bool String::operator == (String string) { return streql(this->Data, string.Data); }
}