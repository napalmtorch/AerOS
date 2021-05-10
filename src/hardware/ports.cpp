#include "hardware/ports.hpp"
#include "core/kernel.hpp"

// port byte in
uint8_t inb(uint16_t port)
{
    uint8_t result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

// port byte out
uint16_t inw(uint16_t port)
{
    uint16_t result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

// port long int
uint32_t inl(uint16_t port)
{
    uint32_t rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
    return rv;
}
void insw(unsigned short port, unsigned char * data, unsigned long size) {
	asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}
void outsw(unsigned short port, unsigned char * data, unsigned long size) {
	asm volatile ("rep outsw" : "+S" (data), "+c" (size) : "d" (port));
}
// port byte out
void outb(uint16_t port, uint8_t data) { __asm__ __volatile__("out %%al, %%dx" : : "a" (data), "d" (port)); }

// port word out
void outw(uint16_t port, uint16_t data) { __asm__ __volatile__("out %%ax, %%dx" : : "a" (data), "d" (port)); }

// port long out
void outl(uint16_t port, uint32_t data) { __asm__ __volatile__("outl %%eax, %%dx" : : "dN" (port), "a" (data)); }

   // constructor - default
    IOPort::IOPort() { this->Port = 0; }

    // constructor - selected port
    IOPort::IOPort(uint16_t port) { this->Port = port; }

    // write 8-bit value to port
    void IOPort::WriteInt8(uint8_t data)
    {
        if (Port == 0) { System::KernelIO::ThrowError("Tried to write 8-bit value to null port"); return; }
        outb(Port, data);
    }

    // write 16-bit value to port
    void IOPort::WriteInt16(uint16_t data)
    {
        if (Port == 0) { System::KernelIO::ThrowError("Tried to write 16-bit value to null port"); return; }
        outw(Port, data);
    }

    // write 32-bit value to port
    void IOPort::WriteInt32(uint32_t data)
    {
        if (Port == 0) { System::KernelIO::ThrowError("Tried to write 32-bit value to null port"); return; }
        outl(Port, data);
    }

    // read 8-bit value from port
    uint8_t IOPort::ReadInt8()
    {
        if (Port == 0) { System::KernelIO::ThrowError("Tried to read 8-bit value from null port"); return 0; }
        return inb(Port);
    }

    // read 16-bit value from port
    uint16_t IOPort::ReadInt16()
    {
        if (Port == 0) { System::KernelIO::ThrowError("Tried to read 16-bit value from null port"); return 0; }
        return inw(Port);
    }

    // read 32-bit value from port
    uint32_t IOPort::ReadInt32()
    {
        if (Port == 0) { System::KernelIO::ThrowError("Tried to read 32-bit value from null port"); return 0; }
        return inl(Port);
    }

    // get port number
    uint16_t IOPort::GetPort() { return this->Port; }