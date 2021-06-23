#include "hardware/drivers/keyboard.hpp"
#include "core/kernel.hpp"
#include <hardware/interrupt/isr.hpp>
#include <lib/multitasking.h>
#include <lib/string.hpp>

using namespace System;
using namespace System::Threading;
        
System::Threading::Thread::Thread(char* n, char* u,Priority priority, ThreadStart protocol)
{
    initialized = false;
    stack = (void*)(new char[1*1024*1024]);
    regs_state = (registers_t*)((uint32_t)stack + (1*1024*1024) - sizeof(registers_t));

    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(n)) { stradd(name, n[i]); }
    }
    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(u)) { stradd(user, u[i]); }
    }

    regs_state->edi = 0x00;
    regs_state->esi = 0x00;
    regs_state->ebp = 0x00;

    regs_state->eax = 0x00;
    regs_state->ebx = 0x00;
    regs_state->ecx = 0x00;
    regs_state->edx = 0x00;
    
    regs_state->int_no = 0;
    regs_state->error_code = 0;

    this->entry = protocol;

    state.state = State::Halted;

    regs_state->eip = (uint32_t)Thread::handle;
    regs_state->cs = 0x8;
    regs_state->eflags = 0x202;

    if(priority == Priority::High) { Prio = "High"; }
    else if(priority == Priority::Medium) { Prio = "Medium"; }
    else if(priority == Priority::Low) { Prio = "Low"; }
    else if(priority == Priority::Protected) { Prio = "Protected"; }
}

bool System::Threading::Thread::Start()
{
    KernelIO::Write("Started thread: ", COL4_GREEN);
    KernelIO::WriteLine(name);
    KernelIO::TaskManager.LoadThread(this);
    state.state = State::Running;
    initialized = true;
    return true;
}
bool System::Threading::Thread::Stop()
{
    if (!initialized) return false;
    state.state = State::Halted;
    KernelIO::Write("Stopped thread: ", COL4_RED);
    KernelIO::WriteLine(name);
    return true;
}
void System::Threading::Thread::handle()
{
    KernelIO::TaskManager.CurrentThread->entry();
    KernelIO::TaskManager.CurrentThread->state.state = State::Completed;
    KernelIO::TaskManager.UnloadThread(KernelIO::TaskManager.CurrentThread);
    while (true);
}
System::Threading::ThreadState System::Threading::Thread::GetState()
{
    return this->state;
}
void System::Threading::Thread::OnUnhandledException(char* exception)
{
    KernelIO::Terminal.WriteLine("Process with PID %s has encountered an exception and has terminated", this->state.PID, COL4_DARK_RED);
    KernelIO::Terminal.WriteLine(exception, COL4_GRAY);
    state.state = State::Failed;
    KernelIO::TaskManager.UnloadThread(this);
}

System::Threading::ThreadManager::ThreadManager()
{
    loaded_threads = new Thread*[8192];
    HAL::CPU::RegisterIRQ(IRQ0, ThreadManager::thread_switch);
    debug_throw_message(MSG_TYPE_OK, "Initialized task manager");
}

static bool init = false;
void System::Threading::ThreadManager::thread_switch(uint32_t* regs)
{
    KernelIO::Kernel.OnInterrupt();

    if (KernelIO::TaskManager.loaded_threads != nullptr && KernelIO::TaskManager.count > 0)
    {
        registers_t* _regs = (registers_t*)*regs;
        if (KernelIO::TaskManager.count == 0 || KernelIO::TaskManager.loaded_threads == nullptr) return;
        if (init) KernelIO::TaskManager.loaded_threads[KernelIO::TaskManager.CurrentPos]->regs_state = _regs;

        if (++KernelIO::TaskManager.CurrentPos >= KernelIO::TaskManager.count)
            KernelIO::TaskManager.CurrentPos = 0;
        
        Thread* next = KernelIO::TaskManager.loaded_threads[KernelIO::TaskManager.CurrentPos];

        if (next == nullptr) { return; }

        if (next->state.state == State::Failed)
        {
            KernelIO::TaskManager.UnloadThread(next);
            thread_switch(regs);
            return;
        }
        else if (next->state.state == State::Halted)
        {
            thread_switch(regs);
            return;
        }
        
        KernelIO::TaskManager.CurrentThread = next;
        *regs = (uint32_t)(KernelIO::TaskManager.CurrentThread->regs_state);
        init = true;
        //KernelIO::WriteLine("Switching tasks");
    }
}
void System::Threading::ThreadManager::LoadThread(Thread* thread)
{
    if (thread == nullptr) { return; }
    thread->state.PID = count;
    loaded_threads[count] = thread;
    count++;
    KernelIO::WriteLine("Loaded thread");
}
void System::Threading::ThreadManager::UnloadThread(Thread* thread)
{
    if (thread == nullptr) { return; }

    for (size_t i = 0; i < count; i++)
    {
        if (loaded_threads[i] == thread)
        {
            mem_free(thread->stack);
            if (loaded_threads[i] == CurrentThread) { CurrentThread = nullptr; }
            CurrentPos = -1;
            loaded_threads[i] = nullptr;
            delete thread;
            return;
        }
    }

    /*
    if (thread == CurrentThread) CurrentThread = nullptr;
    thread->state.PID = -1;
    Thread** temp = new Thread*[count-1];
    int res = 0;
    for (int i = 0, s = 0; i < count; i++)
    {
        if (loaded_threads[i] != thread)
        {
            temp[s] = loaded_threads[i];
            s++;
        }
        else { res = i; }
    }
    --count;
    mem_free(loaded_threads[res]->stack);
    delete loaded_threads[res];
    delete loaded_threads;
    CurrentPos = -1;
    loaded_threads = temp;
    */
}

void System::Threading::ThreadManager::PrintThreads()
{
    for (size_t i = 0; i < count; i++)
    {
        Thread* thread = loaded_threads[i];
        char temp[32];
        strdec((int32_t)thread->state.PID, temp);
        KernelIO::Terminal.Write("PID: ", Graphics::Colors::Yellow);
        KernelIO::Terminal.Write(temp);
        KernelIO::Terminal.Write("  NAME: ", Graphics::Colors::Cyan);
        KernelIO::Terminal.Write(thread->name);
        KernelIO::Terminal.Write("  PRIORITY: ", Graphics::Colors::Cyan);
        KernelIO::Terminal.Write(thread->Prio);
        KernelIO::Terminal.Write("  USER: ", Graphics::Colors::Magenta);
        KernelIO::Terminal.WriteLine(thread->user);
    }
}
bool System::Threading::ThreadManager::ThreadRunning(char* name)
{
    for (size_t i = 0; i < count; i++)
    {
        Thread* thread = loaded_threads[i];
        if(streql(thread->name,name)) { return true; }
    }
    return false;
}
bool System::Threading::ThreadManager::CheckKillPriority(char* name)
{
    for (size_t i = 0; i < count; i++)
    {
        Thread* thread = loaded_threads[i];
        if(streql(thread->Prio,"Protected")) { return false; }
    }
    return true;
}
bool System::Threading::ThreadManager::KillRunning(char* name)
{
    for (size_t i = 0; i < count; i++)
    {
        Thread* thread = loaded_threads[i];
        if(streql(thread->name,name)) { thread->Stop(); thread->state.state = State::Stopped; return true; }
    }
    return false;
}
bool System::Threading::ThreadManager::CanKill(char* name)
{
    for (size_t i = 0; i < count; i++)
    {
        Thread* thread = loaded_threads[i];
        if(streql(thread->user,"aeros")) { return true; }
    }
    return false;
}

List<uint64_t> System::Threading::ThreadManager::GetPids()
{
    List<uint64_t> result;
    for (int i = 0; i < count; i++)
    {
        result.Add(loaded_threads[i]->state.PID);
    }
    return result;
}
System::Threading::Thread* System::Threading::ThreadManager::GetCurrentThread()
{
    return CurrentThread;
}

int32_t System::Threading::ThreadManager::GetThreadCount() { return count; }

extern "C"
{
    System::Threading::Thread* tinit(char* n, char* u,Priority priority ,System::Threading::ThreadStart protocol)
    {
        auto thread = new System::Threading::Thread(n, u,priority, protocol);
        return thread;
    }
    bool tstart(System::Threading::Thread* thread)
    {
        return thread->Start();
    }
    bool tstop(System::Threading::Thread* thread)
    {
        return thread->Stop();
    }
    System::Threading::ThreadState tstate(System::Threading::Thread* thread)
    {
        return thread->GetState();
    }
    uint64_t* tget_pids()
    {
        return KernelIO::TaskManager.GetPids().ToArray();
    }
    System::Threading::Thread* tget_current()
    {
        return KernelIO::TaskManager.GetCurrentThread();
    }
}