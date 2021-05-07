#pragma once
extern "C"
{
    static inline void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
    void CpuDetect();
}
namespace HAL
{
    class CPUInfo
    {
    public: 
     void CPUDetect();
    private:
    };
}