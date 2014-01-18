        .set SRC_SCR,   0x00
        .set SRC_SBMR,  0x04
        .set SRC_SRSR,  0x08
        .set SRC_SISR,  0x14
        .set SRC_SIMR,  0x18
        .set SRC_BASE,  0x53FD0000

        .set BOOTCFG1_MASK,      0x000000FF
        .set BOOTCFG2_MASK,      0x0000FF00
        .set BOOTCFG3_MASK,      0x00FF0000
        .set BMOD_MASK,          0x03000000
        .set BT_FUSE_SEL_MASK,   0x04000000
        .set TEST_MODE_MASK,     0x38000000

        .set BOOTDEVICE_NOR_EIM,      0x1
        .set BOOTDEVICE_RESERVED,     0x2
        .set BOOTDEVICE_HARD_DISK,    0x3
        .set BOOTDEVICE_SROM,         0x4
        .set BOOTDEVICE_SD_ESD,       0x5
        .set BOOTDEVICE_MMC_EMMC,     0x6
        .set BOOTDEVICE_NAND,         0x7
        .set BOOTDEVICE_MASK,         0xF0

