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
     .set ESDHC_CMDRSP3    , 0x1c  @@ ESDHCv2 Command Response 3
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

     .set MMC_RSP_PRESENT,0b1
     .set MMC_RSP_136,    0b10		@@ 136 bit response
     .set MMC_RSP_CRC,    0b100		@@ expect valid crc
     .set MMC_RSP_BUSY,   0b1000		@@ card may send busy
     .set MMC_RSP_OPCODE, 0b10000		@@ response contains opcode

     .set IRQSTATEN   	,	0x0002e034
     .set IRQSTATEN_DMAE	,	(0x10000000)
     .set IRQSTATEN_AC12E	,	(0x01000000)
     .set IRQSTATEN_DEBE	,	(0x00400000)
     .set IRQSTATEN_DCE	,	(0x00200000)
     .set IRQSTATEN_DTOE	,	(0x00100000)
     .set IRQSTATEN_CIE	,	(0x00080000)
     .set IRQSTATEN_CEBE	,	(0x00040000)
     .set IRQSTATEN_CCE	,	(0x00020000)
     .set IRQSTATEN_CTOE	,	(0x00010000)
     .set IRQSTATEN_CINT	,	(0x00000100)
     .set IRQSTATEN_CRM	,	(0x00000080)
     .set IRQSTATEN_CINS	,	(0x00000040)
     .set IRQSTATEN_BRR	,	(0x00000020)
     .set IRQSTATEN_BWR	,	(0x00000010)
     .set IRQSTATEN_DINT	,	(0x00000008)
     .set IRQSTATEN_BGE	,	(0x00000004)
     .set IRQSTATEN_TC	,	(0x00000002)
     .set IRQSTATEN_CC	,	(0x00000001)

     .set  SDHCI_IRQ_EN_BITS, (IRQSTATEN_CC | IRQSTATEN_TC |IRQSTATEN_BWR | IRQSTATEN_BRR | IRQSTATEN_CINT |IRQSTATEN_CTOE | IRQSTATEN_CCE | IRQSTATEN_CEBE | IRQSTATEN_CIE | IRQSTATEN_DTOE | IRQSTATEN_DCE | IRQSTATEN_DEBE)

     .set PRSSTAT_DAT0	,	(0x01000000)
     .set PRSSTAT_CLSL	,	(0x00800000)
     .set PRSSTAT_WPSPL	,	(0x00080000)
     .set PRSSTAT_CDPL	,	(0x00040000)
     .set PRSSTAT_CINS	,	(0x00010000)
     .set PRSSTAT_BREN	,	(0x00000800)
     .set PRSSTAT_SDSTB	,	(0x00000008)
     .set PRSSTAT_DLA	,	(0x00000004)
     .set PRSSTAT_CICHB	,	(0x00000002)
     .set PRSSTAT_CIDHB	,	(0x00000001)

     .set IRQSTAT_DMAE,         (0x10000000)
     .set IRQSTAT_AC12E,    	(0x01000000)
     .set IRQSTAT_DEBE,     	(0x00400000)
     .set IRQSTAT_DCE,      	(0x00200000)
     .set IRQSTAT_DTOE,     	(0x00100000)
     .set IRQSTAT_CIE,      	(0x00080000)
     .set IRQSTAT_CEBE,     	(0x00040000)
     .set IRQSTAT_CCE,      	(0x00020000)
     .set IRQSTAT_CTOE,     	(0x00010000)
     .set IRQSTAT_CINT,     	(0x00000100)
     .set IRQSTAT_CRM,      	(0x00000080)
     .set IRQSTAT_CINS,     	(0x00000040)
     .set IRQSTAT_BRR,      	(0x00000020)
     .set IRQSTAT_BWR,      	(0x00000010)
     .set IRQSTAT_DINT,     	(0x00000008)
     .set IRQSTAT_BGE,      	(0x00000004)
     .set IRQSTAT_TC	,      (0x00000002)
     .set IRQSTAT_CC	,      (0x00000001)

     .set CMD_FINISHED, (IRQSTAT_CC|IRQSTAT_CTOE)
     .set CMD_ERR,      (IRQSTAT_CIE | IRQSTAT_CEBE | IRQSTAT_CCE)
     .set DATA_ERR,	(IRQSTAT_DEBE | IRQSTAT_DCE | IRQSTAT_DTOE)
