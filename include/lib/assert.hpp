#pragma once
#include <lib/types.h>

extern "C"
{
    void assert_true(const char *expr, bool result, const char *file, unsigned line);
    void assert_eql_char(const char *expr, char result, char expected, const char *file, unsigned line);
    void assert_eql_hex(const char *expr, unsigned short result, unsigned short expected, unsigned width, const char *file, unsigned line);
    void assert_eql_mem(const char *expr, void *result, void *expected, size_t len, const char *file, unsigned line);
    void assert_eql_ptr(const char *expr, void *result, void *expected, const char *file, unsigned line);
    void assert_eql_str(const char *expr, const char *result, const char *expected, const char *file, unsigned line);
    void assert_eql_int(const char *expr, long long result, long long expected, const char *file, unsigned line);
    void assert_eql_uint(const char *expr, unsigned long long result, unsigned long long expected, const char *file, unsigned line);

    #define ASSERT_TRUE(expr)       assert_true(#expr, expr, __FILE__, __LINE__)
    #define ASSERT_EQ_CHAR(x, y)    assert_eql_char(#x, x, y, __FILE__, __LINE__)
    #define ASSERT_EQ_HEX8(x, y)    assert_eql_hex(#x, x, y, 2, __FILE__, __LINE__)
    #define ASSERT_EQ_HEX16(x, y)   assert_eql_hex(#x, x, y, 4, __FILE__, __LINE__)
    #define ASSERT_EQ_HEX32(x, y)   assert_eql_hex(#x, x, y, 8, __FILE__, __LINE__)
    #define ASSERT_EQ_HEX64(x, y)   assert_eql_hex(#x, x, y, 16, __FILE__, __LINE__)
    #define ASSERT_EQ_PTR(x, y)     assert_eql_ptr(#x, x, y, __FILE__, __LINE__)
    #define ASSERT_EQ_STR(x, y)     assert_eql_str(#x, x, y, __FILE__, __LINE__)
    #define ASSERT_EQ_INT(x, y)     assert_eql_int(#x, x, y, __FILE__, __LINE__)
    #define ASSERT_EQ_UINT(x, y)    assert_eql_uint(#x, x, y, __FILE__, __LINE__)
}

// ------------------------------------------------------------------------------------------------

namespace Assert
{
    // expected value structure
    typedef struct ExpectedValue
    {
        struct ExpectedValue *next;
        union { char ch; int n; } u;
    } ExpectedValue;

    // expected queue structure
    typedef struct ExpectedQueue
    {
        struct ExpectedQueue *next;
        const char *function;
        const char *param;
        ExpectedValue *head;
        ExpectedValue **tail;
    } ExpectedQueue;

    void InitializeQueue(ExpectedQueue *q, const char *function, const char *param);
    void MockFailure();
    void MatchInt(ExpectedQueue *q, int n);
    void MatchChar(ExpectedQueue *q, char ch);
    void ExpectChar(ExpectedQueue *q, char ch);
    void ExpectInt(ExpectedQueue *q, int n);
    void EndQueue(ExpectedQueue *q);
    void PreMock(const char *file, unsigned line);
    void PostMock();

    void IsTrue(const char *expr, bool result, const char *file, unsigned line);
    void EqualsChar(const char *expr, char result, char expected, const char *file, unsigned line);
    void EqualsHex(const char *expr, unsigned short result, unsigned short expected, unsigned width, const char *file, unsigned line);
    void EqualsMem(const char *expr, void *result, void *expected, size_t len, const char *file, unsigned line);
    void EqualsPointer(const char *expr, void *result, void *expected, const char *file, unsigned line);
    void EqualsString(const char *expr, const char *result, const char *expected, const char *file, unsigned line);
    void EqualsInteger(const char *expr, long long result, long long expected, const char *file, unsigned line);
    void EqualsUInteger(const char *expr, unsigned long long result, unsigned long long expected, const char *file, unsigned line);

    #define MATCH_CHAR(function, param) MatchChar(&Mock.function##param, param)
    #define MATCH_INT(function, param) MatchInt(&Mock.function##param, param)

    #define EXPECT_CHAR(function, param) ExpectChar(&Mock.function##param, param)
    #define EXPECT_INT(function, param) ExpectInt(&Mock.function##param, param)

    #define DECLARE_QUEUE(function, param) ExpectedQueue function##param

    #define INIT_QUEUE(function, param) InitQueue(&Mock.function##param, #function, #param)

    #define MOCK_BEGIN PreMock(__FILE__, __LINE__)
    #define MOCK_END PostMock()

    #define RUN_MOCK(x)  MOCK_BEGIN; x; MOCK_END
    #define EXPECT(x) expect_##x

}