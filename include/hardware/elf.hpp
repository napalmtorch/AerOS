#pragma once
#include <lib/types.h>
namespace ELF
{

class Loader
{
typedef struct
{
    unsigned char ident[16];
    unsigned short type;
    unsigned short machine;
    unsigned int version;
    unsigned long entry;
    unsigned long phoff;
    unsigned long sphoff;
    unsigned int flags;
    unsigned short ehsize;
    unsigned short phentsize;
    unsigned short phnum;
    unsigned short shentsize;
    unsigned short shnum;
    unsigned short shstrndx;
} elfHeader;

/*
 * An entry in the ELF program header table.
 * This describes a single segment of the executable.
 */
/*
 typedef struct {
 unsigned int type;
 unsigned int offset;
 unsigned long vaddr;
 unsigned long paddr;
 unsigned long fileSize;
 unsigned long memSize;
 unsigned long flags;
 unsigned long alignment;
 } programHeader;

 */

typedef struct
{
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t fileSize;
    uint64_t memSize;
    uint64_t alignment;
}__attribute__((packed)) programHeader;

/*
 * Bits in flags field of programHeader.
 * These describe memory permissions required by the segment.
 */
#define PF_R        0x4 /* Pages of segment are readable. */
#define PF_W        0x2 /* Pages of segment are writable. */
#define PF_X        0x1 /* Pages of segment are executable. */

/*
 * A segment of an executable.
 * It specifies a region of the executable file to be loaded
 * into memory.
 */
struct Exe_Segment
{
    uint64_t offsetInFile; /* Offset of segment in executable file */
    uint64_t lengthInFile; /* Length of segment data in executable file */
    uint64_t startAddress; /* Start address of segment in user memory */
    uint64_t sizeInMemory; /* Size of segment in memory */
    uint64_t vaddr;
    int protFlags; /* VM protection flags; combination of VM_READ,VM_WRITE,VM_EXEC */
};

/*
 * Maximum number of executable segments we allow.
 * Normally, we only need a code segment and a data segment.
 * Recent versions of gcc (3.2.3) seem to produce 3 segments.
 */
#define EXE_MAX_SEGMENTS 100

/*
 * A struct concisely representing all information needed to
 * load an execute an executable.
 */
struct Exe_Format
{
    struct Exe_Segment segmentList[EXE_MAX_SEGMENTS]; /* Definition of segments */
    int numSegments; /* Number of segments contained in the executable */
    uint64_t entryAddr; /* Code entry point address */
};

int parse_ELF(char *exeFileData, uint64_t exeFileLength,
        struct Exe_Format *exeFormat, programHeader * pdr);

void print_EXEformat(struct Exe_Format* exe_format);

};
}
