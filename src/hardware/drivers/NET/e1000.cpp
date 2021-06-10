#include <hardware/drivers/NET/e1000.hpp>
#include <core/kernel.hpp>
namespace System
{
    namespace Network
    {
        e1000::e1000()
        {
            Initialize();
        }

        void e1000::Initialize()
        {
            System::KernelIO::Terminal.Write("Network: ",COL4_YELLOW);
            System::KernelIO::Terminal.WriteLine("Initializing e1000 driver",COL4_MAGENTA);
        }

        void e1000::PollData() { //this runs threaded in "core"
           auto net_thread = tinit("Network","aeros",System::Threading::Priority::Medium,[]() {
            while(true);
           });
          // tstart(net_thread);
                
        }

        e1000::~e1000() {
        }
    }
}