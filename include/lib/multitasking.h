#pragma once
#include <lib/types.h>
#include <lib/list.hpp>
#include <hardware/interrupt/isr.hpp>

#ifdef __cplusplus
namespace System
{
    namespace Threading
    {
        typedef void (*ThreadStart)();
        enum class State
        {
            Running,
            Halted,     // stopped, but can be restored
            Failed,     // failed, the thread got an unhandled exception, not restorable
            Completed
        };
        struct ThreadState
        {
            State state;
            uint64_t PID;
        };
        class Thread
        {
        friend class ThreadManager;
        private:
            void*        stack;
            registers_t* regs_state;
            ThreadState  state;
            static void  handle();
            ThreadStart  entry;
            bool         initialized;
        public:
            Thread(ThreadStart protocol);

            bool Start();
            bool Stop();

            ThreadState GetState();
        };
        class ThreadManager
        {
        friend class Thread;
        private:
            uint32_t CurrentPos = 0;
            Thread* CurrentThread = NULL;
            List<Thread*> loaded_threads = List<Thread*>();
            static void thread_switch(uint32_t* regs);
            void LoadThread(Thread* thread);
            void UnloadThread(Thread* thread);
        public:
            Thread*           GetCurrentThread();
            List<uint64_t>    GetPids();
            ThreadManager();
        };
    }
}

extern "C"
{
    System::Threading::Thread*     tinit       (System::Threading::ThreadStart protocol);
    bool                           tstart      (System::Threading::Thread* thread);
    bool                           tstop       (System::Threading::Thread* thread);
    System::Threading::ThreadState tstate      (System::Threading::Thread* thread);
    uint64_t*                      tget_pids   ();
    System::Threading::Thread*     tget_current();
}
#else
#endif