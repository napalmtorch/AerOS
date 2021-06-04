#include "hardware/drivers/keyboard.hpp"
#include <apps/win_term.hpp>
#include "core/kernel.hpp"
#include <hardware/interrupt/isr.hpp>
#include <lib/multitasking.h>
#include <lib/string.hpp>

using namespace System;
using namespace System::Threading;
        
System::Threading::Thread::Thread(ThreadStart protocol)
{
    stack = (void*)(new char[1*1024*1024]);
    regs_state = (registers_t*)((uint32_t)stack + (1*1024*1024) - sizeof(registers_t));
    
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
}

bool System::Threading::Thread::Start()
{
    if (initialized) return false;
    KernelIO::TaskManager.LoadThread(this);
    state.state = State::Running;
    initialized = true;
    return true;
}
bool System::Threading::Thread::Stop()
{
    if (!initialized) return false;
    state.state = State::Halted;
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

System::Threading::ThreadManager::ThreadManager()
{
    HAL::CPU::RegisterIRQ(IRQ0, ThreadManager::thread_switch);
    debug_writeln("Registered ThreadManager");
}

static bool init = false;
void System::Threading::ThreadManager::thread_switch(uint32_t* regs)
{
    registers_t* _regs = (registers_t*)*regs;
    if (KernelIO::TaskManager.loaded_threads.Count == 0 || KernelIO::TaskManager.loaded_threads.ToArray() == nullptr) return;
    if (init) (*KernelIO::TaskManager.loaded_threads.Get(KernelIO::TaskManager.CurrentPos))->regs_state = _regs;

    if (++KernelIO::TaskManager.CurrentPos >= KernelIO::TaskManager.loaded_threads.Count)
        KernelIO::TaskManager.CurrentPos = 0;
    Thread* next = (*KernelIO::TaskManager.loaded_threads.Get(KernelIO::TaskManager.CurrentPos));

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
}
void System::Threading::ThreadManager::LoadThread(Thread* thread)
{
    thread->state.PID = loaded_threads.Count;
    loaded_threads.Add(thread);
    KernelIO::ThrowOK("loaded thread");
}
void System::Threading::ThreadManager::UnloadThread(Thread* thread)
{
    thread->state.PID = -1;
    loaded_threads.Remove(thread);
}
List<uint64_t> System::Threading::ThreadManager::GetPids()
{
    List<uint64_t> result;
    for (int i = 0; i < loaded_threads.Count; i++)
    {
        result.Add((*loaded_threads.Get(i))->state.PID);
    }
    return result;
}
System::Threading::Thread* System::Threading::ThreadManager::GetCurrentThread()
{
    return CurrentThread;
}

extern "C"
{
    System::Threading::Thread* tinit(System::Threading::ThreadStart protocol)
    {
        auto thread = new System::Threading::Thread(protocol);
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