#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/interrupt/idt.hpp"
#include "hardware/interrupt/isr.hpp"

// Kernel IO API by napalmtorch and Nik300

// main namespace of the System library - here i am
#ifdef __cplusplus
namespace System
{
    namespace KernelIO
    {
        // kernel core
        class KernelBase
        {
            public:
                // called as first function before kernel run
                void Initialize();
                
                // kernel core code, runs in a loop
                void Run();
                
                // triggered when a kernel panic is injected
                void OnPanic();
                
                // called when a handled interrupt call is finished
                void OnInterrupt();
                
                // called when interrupt 0x80 is triggered
                void OnSystemCall();
        };

        // kerel base class
        extern KernelIO::KernelBase Kernel;

    }
}
#else

#endif