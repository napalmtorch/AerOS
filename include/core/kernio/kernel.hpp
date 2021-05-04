// required files
#include "lib/types.h"

// Kernel IO API by napalmtorch and Nik300
// [QUOTE: napalmtorch] "honestly man I low key care about my code being pretty more than being practical LMFAO"

//main namespace of the System library
#ifdef __cplusplus
namespace System
{
    namespace KernelIO
    {
        //kernel core
        class KernelBase
        {
            public:
                //called as first function before kernel run
                void Initialize();
                
                //kernel core code, runs in a loop
                void Run();
                
                //triggered when a kernel panic is injected
                void OnPanic();
                
                //called when a handled interrupt call is finished
                void OnInterrupt();
                
                //called when interrupt 0x80 is triggered
                void OnSystemCall();
        };

        // system message types
        typedef enum
        {
            MSG_TYPE_OK,
            MSG_TYPE_SYSTEM,
            MSG_TYPE_WARNING,
            MSG_TYPE_ERROR,
        } MSG_TYPE;
        

        // debugging
        void ThrowMessage(MSG_TYPE type, char* msg);
        void ThrowOK(char* msg);
        void ThrowSystem(char* msg);
        void ThrowError(char* msg);
        void ThrowWarning(char* msg);
        void ThrowPanic(char* msg);
        void Write(char* text);
        void Write(char* text, uint8_t color);
        void WriteLine(char* text);
        void WriteLine(char* text, uint8_t color);
    }

    // kerel base class
    extern KernelIO::KernelBase Kernel;
}
#else
void debug_throw_message(MSG_TYPE type, char* msg);
void debug_throw_panic(char* msg);
void Write(char* text);
void Write(char* text, uint8_t color);
void WriteLine(char* text);
void WriteLine(char* text, uint8_t color);
#endif