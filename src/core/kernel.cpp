#include "core/kernio/kernel.hpp"

namespace System
{
    namespace KernelIO
    {
        //called as first function before kernel run
        void KernelBase::Initialize()
        {

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