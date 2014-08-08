#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define HEADER_TAG 0x402000d1
#define IVT_OFFSET 0x400

struct ivt
{
  uint32_t header;
  uint32_t entry;
  uint32_t reserved1;
  uint32_t dcd;
  uint32_t boot_data;
  uint32_t self;
  uint32_t csf;
  uint32_t reserved2;
};

struct bootdata
{
  uint32_t start;
  uint32_t length;
  uint32_t plugin_flag;
};

void usage ()
{
  fprintf (stderr,"ivtgen: Image Vector Table generator\n"
           "Usage:\n"
           "        ./ivtgen <base_address> <img_size> <entry>\n"
           "\nArguments:\n"
           "        1 - Base address.\n"
           "        2 - Image Size.\n"
           "        3 - Entry point.\n"
           "\nReport bugs to gabriel@krisman.be\n");
}

int main (int argc, char **argv)
{
  struct ivt ivt;
  struct bootdata bootdata;
  uint32_t entry;
  uint32_t base_address;
  uint32_t img_size;

  if (argc < 3)
    {
      usage ();
      return 0;
    }

  sscanf (argv[1], "%x", &base_address);
  sscanf (argv[2], "%x", &img_size);
  sscanf (argv[3], "%x", &entry);

  ivt.header = HEADER_TAG;
  ivt.entry = entry;
  ivt.reserved1 = 0x0;
  ivt.dcd = 0x0;
  ivt.boot_data = base_address + IVT_OFFSET + sizeof(ivt);
  ivt.self = base_address + IVT_OFFSET;
  ivt.csf = 0x0;
  ivt.reserved2 = 0x0;

  bootdata.start = base_address;
  bootdata.length = img_size;

  write(1, (void *)&ivt, sizeof (ivt));
  write(1, (void *)&bootdata, sizeof (bootdata));
  return 0;
}
