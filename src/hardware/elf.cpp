#include <lib/types.h>
#include <hardware/elf.hpp>
#include <core/kernel.hpp>
namespace ELF
{

int elfDebug = 0;
int strncmp(const char *s1, const char *s2, uint32_t n)
{
    for (; n > 0; s1++, s2++, --n)
        if (*s1 != *s2)
            return ((*(unsigned char *) s1 < *(unsigned char *) s2) ? -1 : +1);
        else if (*s1 == '\0')
            return 0;
    return 0;
}
int Loader::parse_ELF(char *exeFileData, uint64_t exeFileLength,
        struct Exe_Format *exeFormat, programHeader * phdr)
{
    elfHeader *hdr;
    int i;
    hdr = (elfHeader *) exeFileData;
    if (exeFileLength < sizeof(elfHeader)
            || strncmp(exeFileData, "\x7F" "ELF", 4) != 0)
    {
        return 1;
    }
    if (hdr->phnum > EXE_MAX_SEGMENTS)
    {
        return 1;
    }
    if (exeFileLength < hdr->phoff + (hdr->phnum * sizeof(programHeader)))
    {
        return 1;
    }
      System::KernelIO::WriteLineDecimal("Getting from Elf: ",hdr->phnum);
    exeFormat->numSegments = hdr->phnum;
    exeFormat->entryAddr = hdr->entry;
    phdr = (programHeader *) (exeFileData + hdr->phoff);
    for (i = 0; i < hdr->phnum; ++i)
    {
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        segment->offsetInFile = phdr[i].offset;
        segment->lengthInFile = phdr[i].fat_get_file_size;
        segment->sizeInMemory = phdr[i].memSize;
        segment->vaddr = phdr[i].paddr;
        if (segment->lengthInFile > segment->sizeInMemory)
        {
            return 1;
        }
    }
    return 0;
}
void Loader::print_EXEformat(struct Exe_Format* exe_format)
{
    int i = 0;
      System::KernelIO::WriteLineDecimal("Num Segment of Exe Format : %d", exe_format->numSegments);
    for (; i < exe_format->numSegments; i++)
    {
        System::KernelIO::WriteDecimal("Segments: ",exe_format->segmentList[i].offsetInFile);
        System::KernelIO::WriteDecimal(" : ",exe_format->segmentList[i].lengthInFile);
        System::KernelIO::WriteDecimal(" : ",exe_format->segmentList[i].startAddress);
        System::KernelIO::WriteLineDecimal(" : ",exe_format->segmentList[i].sizeInMemory);
    }
}
}
