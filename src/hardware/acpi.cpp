//
// here is the slighlty complicated ACPI poweroff and APIC code
//

#define APIC_TYPE_LOCAL_APIC            0
#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

#include <core/kernel.hpp>
#include <lib/types.h>
#include <hardware/memory.hpp>
#include <lib/string.hpp>

extern "C" {
dword *SMI_CMD;
byte ACPI_ENABLE;
byte ACPI_DISABLE;
dword *PM1a_CNT;
dword *PM1b_CNT;
word SLP_TYPa;
word SLP_TYPb;
word SLP_EN;
word SCI_EN;
byte PM1_CNT_LEN;

/* keyboard interface IO port: data and control
   READ:   status port
   WRITE:  control register */
#define KBRD_INTRFC 0x64
 
/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */
 
#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */
 
#define bit(n) (1<<(n)) /* Set bit n to 1 */
 
/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))

struct RSDPtr
{
   byte Signature[8];
   byte CheckSum;
   byte OemID[6];
   byte Revision;
   dword *RsdtAddress;
};



struct FACP
{
   byte Signature[4];
   dword Length;
   byte unneded1[40 - 8];
   dword *DSDT;
   byte unneded2[48 - 44];
   dword *SMI_CMD;
   byte ACPI_ENABLE;
   byte ACPI_DISABLE;
   byte unneded3[64 - 54];
   dword *PM1a_CNT_BLK;
   dword *PM1b_CNT_BLK;
   byte unneded4[89 - 72];
   byte PM1_CNT_LEN;
};

struct AcpiHeader
{
    uint32_t signature;
    uint32_t length;
    uint32_t revision;
    uint32_t checksum;
    uint32_t oem[6];
    uint32_t oemTableId[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
} __attribute__((packed));


struct AcpiMadt
{
    AcpiHeader header;
    uint32_t localApicAddr;
    uint32_t flags;
} __attribute__((packed));

struct ApicHeader
{
    uint8_t type;
    uint8_t length;
} __attribute__((packed));

struct ApicLocalApic
{
    ApicHeader header;
    uint8_t acpiProcessorId;
    uint8_t apicId;
    uint32_t flags;
} __attribute__((packed));



// check if the given address has a valid header
unsigned int *acpiCheckRSDPtr(unsigned int *ptr)
{
   char *sig = "RSD PTR ";
   struct RSDPtr *rsdp = (struct RSDPtr *) ptr;
   byte *bptr;
   byte check = 0;
   int i;

   if (memcmp(sig, rsdp, 8) == 0)
   {
      // check checksum rsdpd
      bptr = (byte *) ptr;
      for (i=0; i<sizeof(struct RSDPtr); i++)
      {
         check += *bptr;
         bptr++;
      }

      // found valid rsdpd   
      if (check == 0) {
         /*
          if (desc->Revision == 0)
            wrstr("acpi 1");
         else
            wrstr("acpi 2");
         */
         return (unsigned int *) rsdp->RsdtAddress;
      }
   }

   return NULL;
}



// finds the acpi header and returns the address of the rsdt
uint8_t *acpiGetRSDPtr(void)
{
   // TODO - Search Extended BIOS Area

   // Search main BIOS area below 1MB
   uint8_t *p   = (uint8_t *)0x000e0000;
   uint8_t *end = (uint8_t *)0x000fffff;

   while (p < end)
   {
      uint64_t signature = *(uint64_t *)p;
      if (signature == 0x2052545020445352) // 'RSD PTR '
      {
         if (acpiCheckRSDPtr((unsigned int*)p)) return p;
      }
      p += 16;
   }
   return NULL;
}



// checks for a given header and validates checksum
int acpiCheckHeader(unsigned int *ptr, char *sig)
{
   if (memcmp(ptr, sig, 4) == 0)
   {
      char *checkPtr = (char *) ptr;
      int len = *(ptr + 1);
      char check = 0;
      while (0<len--)
      {
         check += *checkPtr;
         checkPtr++;
      }
      if (check == 0)
         return 0;
   }
   return -1;
}
uint32_t jiffies = 0;
uint16_t hz = 0;
AcpiMadt* madt_header = NULL;

void sleep(int sec) {
    uint32_t end = jiffies + sec * hz;
    while(jiffies < end);
}

int acpiEnable()
{
   // check if acpi is enabled
   if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 0 )
   {
      // check if acpi can be enabled
      if (SMI_CMD != 0 && ACPI_ENABLE != 0)
      {
         outb((unsigned int) SMI_CMD, ACPI_ENABLE); // send acpi enable command
         // give 3 seconds time to enable acpi
         int i;
         for (i=0; i<300; i++ )
         {
            if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 1 )
               break;
            sleep(10);
         }
         if (PM1b_CNT != 0)
            for (; i<300; i++ )
            {
               if ( (inw((unsigned int) PM1b_CNT) &SCI_EN) == 1 )
                  break;
               sleep(10);
            }
         if (i<300) {
            System::KernelIO::WriteLine("enabled acpi.\n");
            return 0;
         } else {
            System::KernelIO::WriteLine("couldn't enable acpi.\n");
            return -1;
         }
      } else {
         System::KernelIO::WriteLine("no known way to enable acpi.\n");
         return -1;
      }
   } else {
      //System::KernelIO::WriteLine("acpi was already enabled.\n");
      return 0;
   }
}

static uint8_t* rsdt_ptr = NULL;

void acpiParseDT(AcpiHeader* header)
{
   uint32_t signature = (uint32_t)header->signature;
   
   char s[20];
   strhex(signature, s);

   System::KernelIO::Terminal.WriteLine(s);

   if (acpiCheckHeader((uint32_t*)header->signature, "FACP") == 0)
   {
      System::KernelIO::Terminal.WriteLine("found FACP");
   }
   else if (acpiCheckHeader((uint32_t*)header->signature, "APIC") == 0)
   {
      System::KernelIO::Terminal.WriteLine("found APIC");
   }
}

void acpiParseRsdt(AcpiHeader* rsdt)
{
   uint32_t* p = (uint32_t*)(rsdt + 1);
   uint32_t* end = (uint32_t*)(rsdt + rsdt->length);

   //System::KernelIO::Terminal.WriteLine("%s", rsdt->length);

   while (p < end)
   {
      uint32_t addr = *p++;
      acpiParseDT((AcpiHeader*)addr);
   }
}
void acpiParseXsdt(AcpiHeader* xsdt)
{
   uint64_t* p = (uint64_t*)(xsdt + 1);
   uint64_t* end = (uint64_t*)(xsdt + xsdt->length);

   while (p < end)
   {
      uint64_t addr = *p++;
      acpiParseDT((AcpiHeader*)addr);
   }
}

bool acpiParseRsdp(uint8_t* p)
{
   // checksum
   uint8_t sum = 0;
   for (uint i = 0; i < 20; ++i)
   {
      sum += p[i];
   }

   if (sum)
   {
      System::KernelIO::Terminal.WriteLine("Error: rsdp checksum was not valid!", COL4_DARK_RED);
      return false;
   }

   uint8_t revision = p[15];
   if (revision == 0)
   {
      uint32_t rsdtAddr = *(uint32_t*)(p+16);
      acpiParseRsdt((AcpiHeader*)rsdtAddr);
   }
   else if (revision == 2)
   {
      uint32_t rsdtAddr = *(uint32_t *)(p + 16);
      uint64_t xsdtAddr = *(uint64_t *)(p + 24);

      if(xsdtAddr)
         acpiParseXsdt((AcpiHeader*)xsdtAddr);
      else
         acpiParseRsdt((AcpiHeader*)rsdtAddr);
   }
   else
   {
      System::KernelIO::Terminal.WriteLine("Error: unsupported rsdp revision!", COL4_DARK_RED);
      return false;
   }
   return true;
}

int initAcpi()
{
   // TODO - Search Extended BIOS Area

   // Search main BIOS area below 1MB
   uint8_t *p   = (uint8_t *)0x000e0000;
   uint8_t *end = (uint8_t *)0x000fffff;

   while (p < end)
   {
      uint64_t signature = *(uint64_t *)p;
      if (signature == 0x2052545020445352) // 'RSD PTR '
      {
         if (acpiCheckRSDPtr((unsigned int*)p)){
            if (!acpiParseRsdp(p))
               return -1;
            return 0;
         }
      }
      p += 16;
   }
   return -1;
}

void OldShutdown()
{
   __asm__ __volatile__ ("outw %1, %0" : : "dN" ((uint16_t)0xB004), "a" ((uint16_t)0x2000));
}

void acpiPowerOff(void)
{
   // SCI_EN is set to 1 if acpi shutdown is possible
   if (SCI_EN == 0)
      return;

   acpiEnable();

   // send the shutdown command
   outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
   if ( PM1b_CNT != 0 )
   {
      outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );
   }
   else
   {
   System::KernelIO::WriteLine("acpi poweroff failed. Performing Legacy Shutdown\n");
   OldShutdown();
   }
}
 
void Reboot()
{
    uint8_t temp;
 
    asm volatile ("cli");
 
    do
    {
        temp = inb(KBRD_INTRFC);
        if (check_flag(temp, KBRD_BIT_KDATA) != 0)
            inb(KBRD_IO);
    } while (check_flag(temp, KBRD_BIT_UDATA) != 0);
 
    outb(KBRD_INTRFC, KBRD_RESET);
loop:
    asm volatile ("hlt");
    goto loop;
}
}
namespace HAL
{
   int ACPI::ACPIInit() { return initAcpi(); }
   void ACPI::Shutdown() { return acpiPowerOff(); }
   void ACPI::LegacyShutdown() { return OldShutdown(); }
   void ACPI::Reboot() { return Reboot(); }
   //acpiCheckHeader(ptr, "RSDT")
   MADT* ACPI::GetAPICInfo()
   {
      if (madt_header == NULL) return NULL;
      MADT* madt = new MADT;
      madt->numcore = 0;
      madt->lapic_ptr = madt_header->localApicAddr;
         char s[5];
         strdec(madt_header->localApicAddr, s);
         System::KernelIO::WriteLine(s);
      
      uint8_t *p = (uint8_t *)(madt_header + 1);
      uint8_t *end = (uint8_t *)madt_header + madt_header->header.length;

      while (p<end)
      {
         ApicHeader* header = (ApicHeader*)p;
         uint8_t lenght = header->length;
         char s[5];
         uint8_t type = header->type;
         if (type == APIC_TYPE_LOCAL_APIC)
         {
            ApicLocalApic* lapic = (ApicLocalApic*)p;
            uint8_t* temp = new uint8_t[madt->numcore + 1];
            if(madt->lapic_ids != NULL) mem_copy(madt->lapic_ids, temp, madt->numcore);
            madt->lapic_ids = temp;
            madt->lapic_ids[madt->numcore] = lapic->apicId;
            ++madt->numcore;
         }
         p += lenght;
         break;
      }
      return madt;
   }
}