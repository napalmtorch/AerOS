// ------------------------------------------------------------------------------------------------
// test/test.c
// ------------------------------------------------------------------------------------------------

#include <hardware/assert.hpp>
#include <hardware/memory.hpp>

// ------------------------------------------------------------------------------------------------
static ExpectedQueue *s_params;
static const char *s_file;
static unsigned s_line;

// ------------------------------------------------------------------------------------------------
void AssertTrue(const char *expr, bool result, const char *file, unsigned line)
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

// ------------------------------------------------------------------------------------------------
void AssertEqChar(const char *expr, char result, char expected, const char *file, unsigned line)
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

// ------------------------------------------------------------------------------------------------
void AssertEqHex(const char *expr, unsigned short result, unsigned short expected, unsigned width, const char *file, unsigned line)
{
    if (result != expected)
    {
        System::KernelIO::WriteLine("Test AssertEqHex failed");
    }
      System::KernelIO::WriteLine("Test AssertEqHex Succeeded");
      return;
}

// ------------------------------------------------------------------------------------------------
void AssertEqMem(const char *expr, void *result, void *expected, size_t len, const char *file, unsigned line)
{
    if (memcmp(result, expected, len))
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

// ------------------------------------------------------------------------------------------------
void AssertEqPtr(const char *expr, void *result, void *expected, const char *file, unsigned line)
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

// ------------------------------------------------------------------------------------------------
void AssertEqStr(const char *expr, const char *result, const char *expected, const char *file, unsigned line)
{
    if (strcmp((char*)result, (char*)expected))
    {
    }
}

// ------------------------------------------------------------------------------------------------
void AssertEqInt(const char *expr, long long result, long long expected, const char *file, unsigned line)
{
    if (result != expected)
    {
    }
}

// ------------------------------------------------------------------------------------------------
void AssertEqUint(const char *expr, unsigned long long result, unsigned long long expected, const char *file, unsigned line)
{
    if (result != expected)
    {
    }
}


// ------------------------------------------------------------------------------------------------
void InitQueue(ExpectedQueue *q, const char *function, const char *param)
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
