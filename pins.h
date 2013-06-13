#ifndef PINS_H
#define PINS_H

namespace PINS
{
    /* Set TEST_MODE pins as low.  */
    const bool TEST_MODE[3] = {false, false, false};

    /* Set BT_FUSE_SEL as low since we use eFUSE.  */
    const bool BT_FUSE_SEL =  false;

    /* Set BMOD to use eFUSE.  */
    const bool BMOD[2] = {false, true};

    /* Set BOOT_CFG to show POR and use SD card.  */
    const char BOOT_CFG[3] = {0x40, 0x00, 0x00};
}

#endif /* !PINS_H.  */
