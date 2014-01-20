@------------------------------------------------------------------------
@------------------------------------------------------------------------
@ ESDHCv2-1 address space constants definition.
@------------------------------------------------------------------------

     .set ESDHC_BASE       , 0x50004000
     .set ESDHC_DSADR      , 0x00  @@ ESDHCv2 DMA System Address
     .set ESDHC_BLKATTR    , 0x04  @@ ESDHCv2 Block attribute
     .set ESDHC_CMDARG     , 0x08  @@ ESDHCv2 Command Argument
     .set ESDHC_XFERTYP    , 0x0C  @@ ESDHCv2 Command Transfer type
     .set ESDHC_CMDRSP0    , 0x10  @@ ESDHCv2 Command Response 0
     .set ESDHC_CMDRSP1    , 0x14  @@ ESDHCv2 Command Response 1
     .set ESDHC_CMDRSP2    , 0x18  @@ ESDHCv2 Command Response 2
     .set ESDHC_DATPORT    , 0x20  @@ ESDHCv2 Data buffer access port
     .set ESDHC_PRSSTAT    , 0x24  @@ ESDHCv2 Present State
     .set ESDHC_PROCTL     , 0x28  @@ ESDHCv2 Protocol Control
     .set ESDHC_SYSCTL     , 0x2C  @@ ESDHCv2 System Control
     .set ESDHC_IRQSTAT    , 0x30  @@ ESDHCv2 Interrupt Status
     .set ESDHC_IRQSTATEN  , 0x34  @@ ESDHCv2 Interrupt Status Enable
     .set ESDHC_IRQSIGEN   , 0x38  @@ ESDHCv2 Interrupt signal Enable
     .set ESDHC_AUTOC12ERR , 0x3C  @@ ESDHCv2 Auto CMD12 Error
     .set ESDHC_HOSTCAPBLT , 0x40  @@ ESDHCv2 host Capatibilites
     .set ESDHC_WML        , 0x44  @@ ESDHCv2 Watermark Level
     .set ESDHC_FEVT       , 0x50  @@ ESDHCv2 Force Event
     .set ESDHC_ADMAEST    , 0x54  @@ ESDHCv2 ADMA Error Status
     .set ESDHC_ADSADDR    , 0x58  @@ ESDHCv2 ADMA System Address
     .set ESDHC_VENDOR     , 0xC0  @@ ESDHCv2 Vendor Specific Register
     .set ESDHC_MMCBOOT    , 0xC4  @@ ESDHCv2 Fast boot Reg.
     .set ESDHC_HOSTVER    , 0xFC  @@ ESDHCv2 Host Controller Version

     .set PROCTL_INIT,      0x08800020

     .set SYSCTL_RSTA,      0x01000000

     .set PRSSTAT_CINS,     0x00010000