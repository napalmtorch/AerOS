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
            char         name[64];
            char         user[64];
            void*        stack;
            registers_t* regs_state;
            ThreadState  state;
            static void  handle();
            ThreadStart  entry;
            bool         initialized;
            Thread();
        public:
            Thread(char* n, char* u, ThreadStart protocol);

            bool Start();
            bool Stop();

            void OnUnhandledException(char* exception);

            ThreadState GetState();
        };
        class ThreadManager
        {
        friend class Thread;
        private:
            uint32_t CurrentPos = 0;
            uint32_t count = 0;
            Thread* CurrentThread = NULL;
            Thread** loaded_threads = nullptr;
            static void thread_switch(uint32_t* regs);
            void LoadThread(Thread* thread);
            void UnloadThread(Thread* thread);
        public:
            Thread*           GetCurrentThread();
            List<uint64_t>    GetPids();
            void PrintThreads();
            bool ThreadRunning(char* name);
            bool KillRunning(char* name);
            bool CanKill(char* name);
            ThreadManager();
        };
    }
}

extern "C"
{
    System::Threading::Thread*     tinit       (char* n,char* u, System::Threading::ThreadStart protocol);
    bool                           tstart      (System::Threading::Thread* thread);
    bool                           tstop       (System::Threading::Thread* thread);
    System::Threading::ThreadState tstate      (System::Threading::Thread* thread);
    uint64_t*                      tget_pids   ();
    System::Threading::Thread*     tget_current();
}
#else
#endif