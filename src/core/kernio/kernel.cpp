#include "core/kernio/kernel.hpp"

namespace System
{
    namespace KernelIO
    {
        // kernel instance
        KernelIO::KernelBase Kernel = KernelIO::KernelBase();

        //called as first function before kernel run
        void KernelBase::Initialize()
        {
            uint8_t* vidmem = (uint8_t*)0xB8000;
            vidmem[0] = 'X';
            vidmem[1] = 0x1E;
        }

        //kernel core code, runs in a loop
        void KernelBase::Run()
        {

        }

        //triggered when a kernel panic is injected
        void KernelBase::OnPanic()
        {

        }

        //called when a handled interrupt call is finished
        void KernelBase::OnInterrupt()
        {

        }

        //called when interrupt 0x80 is triggered
        void KernelBase::OnSystemCall()
        {

        }
    }
}