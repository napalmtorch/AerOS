#include <lib/assert.hpp>
#include <hardware/memory.hpp>

extern "C"
{
    // assert true
    void assert_true(const char *expr, bool result, const char *file, unsigned line)
    {
        System::KernelIO::Write("Testing: ");
        System::KernelIO::WriteLine((char*)file);
        if (!result)
        {
        System::KernelIO::WriteLine("Test AssertTrue Failed");
        }
        System::KernelIO::WriteLine("Test AssertTrue Succeeded");
        return;
    }

    // assert equals char
    void assert_eql_char(const char *expr, char result, char expected, const char *file, unsigned line)
    {
        System::KernelIO::Write("Testing: ");
        System::KernelIO::WriteLine((char*)file);
        if (result != expected)
        {
                System::KernelIO::WriteLine("Test AssertEqChar failed");
        }
        System::KernelIO::WriteLine("Test AssertEqChar Succeeded");
        return;
    }

    // assert equals hex
    void assert_eql_hex(const char *expr, unsigned short result, unsigned short expected, unsigned width, const char *file, unsigned line)
    {
        if (result != expected)
        {
            System::KernelIO::WriteLine("Test AssertEqHex failed");
        }
        System::KernelIO::WriteLine("Test AssertEqHex Succeeded");
        return;
    }

    // assert equals mem
    void assert_eql_mem(const char *expr, void *result, void *expected, size_t len, const char *file, unsigned line)
    {
        if (mem_compare(result, expected, len))
        {

            unsigned char *p = (unsigned char *)result;
            unsigned char *q = (unsigned char *)expected;
            for (size_t i = 0; i < len; ++i)
            {
            }
            System::KernelIO::WriteLine("Test memcmp Succeeded");
        return;
        }
    }

    // assert equals pointer
    void assert_eql_ptr(const char *expr, void *result, void *expected, const char *file, unsigned line)
    {
        System::KernelIO::Write("Testing: ");
        System::KernelIO::WriteLine((char*)file);
        if (result != expected)
        {
            System::KernelIO::WriteLine("Test AssertEqPtr failed");
        }
        System::KernelIO::WriteLine("Test AssertEqPtr Succeeded");
        return;
    }

    // assert equals string
    void assert_eql_str(const char *expr, const char *result, const char *expected, const char *file, unsigned line)
    {
        if (strcmp((char*)result, (char*)expected))
        {
        }
    }

    // assert equals integer
    void assert_eql_int(const char *expr, long long result, long long expected, const char *file, unsigned line)
    {
        if (result != expected)
        {
        }
    }

    // assert equals unsigned integer
    void assert_eql_uint(const char *expr, unsigned long long result, unsigned long long expected, const char *file, unsigned line)
    {
        if (result != expected)
        {
        }
    }
}
namespace Assert
{
    static ExpectedQueue *s_params;
    static const char *s_file;
    static unsigned s_line;
    // ------------------------------------------------------------------------------------------------
    void InitializeQueue(ExpectedQueue *q, const char *function, const char *param)
    {
        q->next = s_params;
        q->function = function;
        q->param = param;
        q->head = 0;
        q->tail = &q->head;

        s_params = q;
    }

    // ------------------------------------------------------------------------------------------------
    void MockFailure()
    {

    }

    // ------------------------------------------------------------------------------------------------
    void MatchInt(ExpectedQueue *q, int n)
    {
        if (q->head == 0)
        {
            MockFailure();
        }
        else
        {
            ExpectedValue *v = q->head;
            q->head = v->next;

            if (v->u.n != n)
            {
                MockFailure();
            }
        }
    }

    // ------------------------------------------------------------------------------------------------
    void MatchChar(ExpectedQueue *q, char ch)
    {
        if (q->head == 0)
        {
            MockFailure();
        }
        else
        {
            ExpectedValue *v = q->head;
            q->head = v->next;

            if (v->u.ch != ch)
            {
                MockFailure();
            }

            mem_free(v);
        }
    }

    // ------------------------------------------------------------------------------------------------
    void ExpectChar(ExpectedQueue *q, char ch)
    {
        ExpectedValue *v = (ExpectedValue *)mem_alloc(sizeof(ExpectedValue));
        v->next = 0;
        v->u.ch = ch;
        *q->tail = v;
        q->tail = &v;
    }

    // ------------------------------------------------------------------------------------------------
    void ExpectInt(ExpectedQueue *q, int n)
    {
        ExpectedValue *v = (ExpectedValue *)mem_alloc(sizeof(ExpectedValue));
        v->next = 0;
        v->u.n = n;
        *q->tail = v;
        q->tail = &v;
    }

    // ------------------------------------------------------------------------------------------------
    void EndQueue(ExpectedQueue *q)
    {
        if (q->head)
        {
            MockFailure();
        }

        q->tail = &q->head;
    }

    // ------------------------------------------------------------------------------------------------
    void PreMock(const char *file, unsigned line)
    {
        s_file = file;
        s_line = line;
    }

    // ------------------------------------------------------------------------------------------------
    void PostMock()
    {
        for (ExpectedQueue *q = s_params; q; q = q->next)
        {
            EndQueue(q);
        }
    }

    void IsTrue(const char *expr, bool result, const char *file, unsigned line) { assert_true(expr, result, file, line); }
    void EqualsChar(const char *expr, char result, char expected, const char *file, unsigned line) { assert_eql_char(expr, result, expected, file, line); }
    void EqualsHex(const char *expr, unsigned short result, unsigned short expected, unsigned width, const char *file, unsigned line) { assert_eql_hex(expr, result, expected, width, file, line); }
    void EqualsMem(const char *expr, void *result, void *expected, size_t len, const char *file, unsigned line) { assert_eql_mem(expr, result, expected, len, file, line); }
    void EqualsPointer(const char *expr, void *result, void *expected, const char *file, unsigned line) { assert_eql_ptr(expr, result, expected, file, line); }
    void EqualsString(const char *expr, const char *result, const char *expected, const char *file, unsigned line) { assert_eql_str(expr, result, expected, file, line); }
    void EqualsInteger(const char *expr, long long result, long long expected, const char *file, unsigned line) { assert_eql_int(expr, result, expected, file, line); }
    void EqualsUInteger(const char *expr, unsigned long long result, unsigned long long expected, const char *file, unsigned line) { assert_eql_uint(expr, result, expected, file, line); }

}
