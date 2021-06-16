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

struct RSDPDescriptor {
    char     Signature[8];
    uint8_t  Checksum;
    char     OEMID[6];
    uint8_t  Revision;
    uint32_t RsdtAddress;
  } __attribute__ ((packed));

  struct RSDPDescriptor20 {
    RSDPDescriptor rdsp10;

    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t  ExtendedChecksum;
    uint8_t  reserved[3];
  } __attribute__ ((packed));

  struct SDTHeader {
    char     Signature[4];
    uint32_t Length;
    uint8_t  Revision;
    uint8_t  Checksum;
    char     OEMID[6];
    char     OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;

    uint32_t sigint() const {
      return *(uint32_t*) Signature;
    }
  };

  struct MADTRecord {
    uint8_t type;
    uint8_t length;
    uint8_t data[0];
  };

  struct MADTHeader
  {
    SDTHeader  hdr;
    uint32_t   lapic_addr;
    uint32_t   flags; // 1 = dual 8259 PICs
    MADTRecord record[0];
  };

  struct FACPHeader
  {
    SDTHeader sdt;
    uint32_t  unneded1;
    uint32_t  DSDT;
    uint8_t   unneded2[48 - 44];
    uint32_t  SMI_CMD;
    uint8_t   ACPI_ENABLE;
    uint8_t   ACPI_DISABLE;
    uint8_t   unneded3[64 - 54];
    uint32_t  PM1a_CNT_BLK;
    uint32_t  PM1b_CNT_BLK;
    uint8_t   unneded4[89 - 72];
    uint8_t   PM1_CNT_LEN;
    uint8_t   unneded5[18];
    uint8_t   century;
  };

  struct AddressStructure
  {
    uint8_t  address_space_id; // 0 - system memory, 1 - system I/O
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  reserved;
    uint64_t address;
  };

  struct pci_vendor_t
  {
    uint16_t    ven_id;
    const char* ven_name;
  };

  struct HPET
  {
    uint8_t hardware_rev_id;
    uint8_t comparator_count :5;
    uint8_t counter_size     :1;
    uint8_t reserved         :1;
    uint8_t legacy_replacem  :1;
    pci_vendor_t pci_vendor_id;
    AddressStructure address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
  } __attribute__((packed));

uint32_t jiffies = 0;
uint16_t hz = 0;

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
            System::KernelIO::WriteLine("enabled acpi.");
            return 0;
         } else {
            System::KernelIO::WriteLine("couldn't enable acpi.");
            return -1;
         }
      } else {
         System::KernelIO::WriteLine("no known way to enable acpi.");
         return -1;
      }
   } else {
      System::KernelIO::WriteLine("acpi was already enabled.");
      return 0;
   }
}

static uint8_t* rsdt_ptr = NULL;

constexpr uint32_t bake(char a, char b , char c, char d) {
   return a | (b << 8) | (c << 16) | (d << 24);
}
uint8_t checksum(const char* addr, size_t size)
{
   const char* end = addr + size;
   uint8_t sum = 0;
   while (addr < end) {
      sum += *addr; addr++;
   }
   return sum;
}

uintptr_t apic_base;
LAPIC lapics[256];
IOAPIC ioapics[256];
override_t overrides[256];
nmi_t nmis[256];

size_t cores_counter = 0;
size_t ioapics_counter = 0;
size_t overrides_counter = 0;
size_t nmis_counter = 0;

void acpiReadMADT(char* addr)
{
   System::KernelIO::Terminal.WriteLine("FOUND MADT",COL4_GREEN);
   auto* hdr = (MADTHeader*)addr;

   apic_base = hdr->lapic_addr;
   int len = hdr->hdr.Length - sizeof(MADTHeader);

   const char* ptr = (char*) hdr->record;
   // fuck you kevin for making me write comments!
   // loop until the end of the MADT
   while (len != 0)
   {
      // store the record
      MADTRecord* record = (MADTRecord*)ptr;

      // simple way to check the record type
      switch (record->type)
      {
      case 0:
      {
         // case 0: this is a local apic, so a core. let's store its id in a list
         auto& lapic = *(LAPIC*) record;
         lapics[cores_counter++] = lapic;
      }
         break;
      case 1:
      {
         // case 1: this is a fucking I/O apic, this will be needed for interrupts handling in APIC
         auto& ioapic = *(IOAPIC*) record;
         ioapics[ioapics_counter++] = ioapic;
      }
      break;
      case 2:
      {
         // case 2: requested override of a specific interrupt.
         auto& _override = *(override_t*)record;
         overrides[overrides_counter++] = _override;
      }
      break;
      case 4:
      {
         // case 3: optional override of NMI interrupts, we don't want stupid hardware to stop our os!
         //i am looking at you apple!
         //this is bad practice, but who the fuck cares, i dont!
         auto& nmi = *(nmi_t*)record;
         nmis[nmis_counter++] = nmi;
      }
         break;
      default:
         System::KernelIO::Terminal.WriteLine("Unknown apic resource type: %s", record->type, COL4_RED);
         break;
      }
      len -= record->length;
      ptr += record->length;
   }
}
void acpiReadFACP(char* addr)
{
   //for a explanation of this see here: https://piv.pivpiv.dk/
   auto* facp = (FACPHeader*) addr;
   System::KernelIO::Terminal.WriteLine("FOUND FACP",COL4_GREEN);
}

uintptr_t HPET_base = 0;

void acpiReadRSDT(SDTHeader* rsdt)
{
   int  total = (rsdt->Length - sizeof(SDTHeader)) / 4;
   
   const char* addr = (const char*) rsdt;
   addr += sizeof(SDTHeader);

   constexpr uint32_t APIC_t = bake('A', 'P', 'I', 'C');
   constexpr uint32_t HPET_t = bake('H', 'P', 'E', 'T');
   constexpr uint32_t FACP_t = bake('F', 'A', 'C', 'P');

   while (0 < total)
   {
      auto* sdt = (SDTHeader*)(*(uintptr_t*)addr);
      addr += 4; total--;

      if (sdt == nullptr) continue;

      switch(sdt->sigint())
      {
         case APIC_t:
            acpiReadMADT((char*)sdt);
            break;
         case FACP_t:
            acpiReadFACP((char*)sdt);
            break;
         case HPET_t:
            HPET_base = ((uintptr_t)addr) + sizeof(SDTHeader);
            break;

      }
   }
}

void acpiReadRDSP(RSDPDescriptor20* rdsp)
{
   SDTHeader* rsdt;
   if (rdsp->XsdtAddress)
      rsdt = (SDTHeader*) (uintptr_t) rdsp->XsdtAddress;
   else
      rsdt = (SDTHeader*) (uintptr_t) rdsp->rdsp10.RsdtAddress;
   if (rsdt->Length < sizeof(SDTHeader))
   {
      System::KernelIO::Terminal.WriteLine("ACPI: Root SDT had impossible lenght", COL4_DARK_RED);
      return;
   }
   if (checksum((char*)rsdt, rsdt->Length) != 0)
   {
      System::KernelIO::Terminal.WriteLine("ACPI: Root SDT verification failed", COL4_DARK_RED);
      return;
   }
   acpiReadRSDT(rsdt);
}

int initAcpi()
{
   // "RSD PTR "
    const uint64_t sign = 0x2052545020445352;

    // guess at QEMU location of RDSP
    const auto* guess = (char*) 0xf68c0;
    if (*(uint64_t*) guess == sign) {
      if (checksum(guess, sizeof(RSDPDescriptor)) == 0)
      {
        acpiReadRDSP((RSDPDescriptor20*)guess);
        return 0;
      }
    }

    // search in BIOS area (below 1mb)
    const auto* addr = (char*) 0x000e0000;
    const auto* end  = (char*) 0x000fffff;

    while (addr < end)
    {
      if (*(uint64_t*) addr == sign) {
        // verify checksum of potential RDSP
        if (checksum(addr, sizeof(RSDPDescriptor)) == 0)
        {
          acpiReadRDSP((RSDPDescriptor20*)addr);
          return 0;
        }
      }
      addr += 16;
    }

    System::KernelIO::Terminal.WriteLine("ACPI RDST-search failed\n", COL4_DARK_RED);
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
      System::KernelIO::Terminal.WriteLine("CORES: %s", cores_counter);
      return NULL;
   }
}