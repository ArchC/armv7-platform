// esdhcv2.h
// -
// This represents the ESDHCv2 used in the ARM SoC by Freescale iMX35.
//
// Author : Gabriel Krisman, 16/11/2012

#ifndef ESDHCv2_H
#define ESDHCv2_H

#include <stdint.h> // define types uint32_t, etc
#include "peripheral.h"
#include "tzic.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "sd.h"
#include <queue>

class ESDHCV2_module : public sc_module, public peripheral
{
private:

    enum state
    {
        IDLE,
        HOST_WRITE,
        HOST_READ_TRANSFER,
        HOST_READ_DONE,
        HOST_WRITE_DONE,
    };

    enum irqstat
    {
        IRQ_DMAE   = 28,
        IRQ_AC12E  = 27,
        IRQ_DEBE   = 22,
        IRQ_DCE    = 21,
        IRQ_DTOE   = 20,
        IRQ_CIE    = 19,
        IRQ_CEBE   = 18,
        IRQ_CCE    = 17,
        IRQ_CTOE   = 16,
        IRQ_CINT   = 8,
        IRQ_CRM    =  7,
        IRQ_CINS_int  = 6,
        IRQ_BRR    = 5,
        IRQ_BWR    = 4,
        IRQ_DINT   = 3,
        IRQ_BGE    = 2,
        IRQ_TC     = 1,
        IRQ_CC     = 0
    };

    enum state current_state;

    static const uint32_t DSADR      = 0x00; // ESDHCv2 DMA System Address
    static const uint32_t BLKATTR    = 0x04; // ESDHCv2 Block attribute
    static const uint32_t CMDARG     = 0x08; // ESDHCv2 Command Argument
    static const uint32_t XFERTYP    = 0x0C; // ESDHCv2 Command Transfer type
    static const uint32_t CMDRSP0    = 0x10; // ESDHCv2 Command Response 0
    static const uint32_t CMDRSP1    = 0x14; // ESDHCv2 Command Response 1
    static const uint32_t CMDRSP2    = 0x18; // ESDHCv2 Command Response 2
    static const uint32_t CMDRSP3    = 0x1C; // ESDHCv2 Command Response 3
    static const uint32_t DATPORT    = 0x20; // ESDHCv2 Data buffer access port
    static const uint32_t PRSSTAT    = 0x24; // ESDHCv2 Present State
    static const uint32_t PROCTL     = 0x28; // ESDHCv2 Protocol Control
    static const uint32_t SYSCTL     = 0x2C; // ESDHCv2 System Control
    static const uint32_t IRQSTAT    = 0x30; // ESDHCv2 Interrupt Status
    static const uint32_t IRQSTATEN  = 0x34; // ESDHCv2 Interrupt Status Enable
    static const uint32_t IRQSIGEN   = 0x38; // ESDHCv2 Interrupt signal Enable
    static const uint32_t AUTOC12ERR = 0x3C; // ESDHCv2 Auto CMD12 Error
    static const uint32_t HOSTCAPBLT = 0x40; // ESDHCv2 host Capatibilites
    static const uint32_t WML        = 0x44; // ESDHCv2 Watermark Level
    static const uint32_t FEVT       = 0x50; // ESDHCv2 Force Event
    static const uint32_t ADMAEST    = 0x54; // ESDHCv2 ADMA Error Status
    static const uint32_t ADSADDR    = 0x58; // ESDHCv2 ADMA System Address
    static const uint32_t VENDOR     = 0xC0; // ESDHCv2 Vendor Specific Register
    static const uint32_t MMCBOOT    = 0xC4; // ESDHCv2 Fast boot Reg.
    static const uint32_t HOSTVER    = 0xFC; // ESDHCv2 Host Controller Version
    static const uint32_t ESDHCV_LASTADDR = 0x100;

    unsigned regs[ESDHCV_LASTADDR/4];

    //XFERTYP
    bool DMAEN;
    bool BCEN;
    bool AC12EN;
    bool DTDSEL;
    bool MSBSEL;
    char RSPTYP;
    bool CCCEN;
    bool CICEN;
    bool DPSEL;
    char CMDTYP;
    char CMDINX;
    //--

    //Present State
    char DLSL;
    bool CLSL;
    bool WPSPL;
    bool CDPL;
    bool CINS;
    bool BREN;
    bool BWEN;
    bool RTA;
    bool WTA;
    bool SDOFF;
    bool PEROFF;
    bool HCKOFF;
    bool IPGOFF;
    bool SDSTB;
    bool DLA;
    bool CDIHB;
    bool CIHB;   //Comand Inibith
    //--

    //Protocol control
    bool WECRM;
    bool WECINS;
    bool WECINT;
    bool IABG;
    bool RWCTL;
    bool CREQ;
    bool SABGREQ;
    bool DMAS[2];
    bool CDSS;
    bool CDTL;
    bool EMODE[2];
    bool D3CD;
    bool DTW[2];
    bool LCTL;
    //--

    //System Control
    bool INITA;
    bool RSTD;
    bool RSTC;
    bool RSTA;
    char DTOCV;
    char SDCLKFS;
    char DVS;
    bool SDCLKEN;
    bool PEREN;
    bool HCKEN;
    bool IPGEN;
    //--

    // Watermark Level
    char WR_BRST_LEN;
    char WR_WML;
    char RD_BRST_LEN;
    char RD_WML;
    //--
    //ADMA Error Status
    bool ADMADCE;
    bool  ADMALME;
    char ADMAES;
    //--

    //BLKATTR
    uint16_t BLKCNT;
    uint16_t BLKCNT_BKP; //Since we must restore BLKCNT value after a MCD12 is issued
    uint16_t BLKSIZE;

    static const int ESDHCV2_1_IRQ = 1;

    sd_card* port;
    std::queue<unsigned char> ibuffer;

    // This port is used to send interrupts to the processor
    tzic_module &tzic;

    // Fast read/write don't implement error checking. The bus (or other caller)
    // must ensure the address is valid.
    // Invalid read/writes are treated as no-ops.
    // Unaligned addresses have undefined behavior
    unsigned fast_read(unsigned address);
    void fast_write(unsigned address, unsigned datum);

    void sd_protocol();
    void host_protocol();

public:

    //Wrappers to call fast_read/write with correct parameters
    unsigned read_signal(unsigned address, unsigned offset) {
        uint32_t ret = fast_read(address);
//        printf("data: 0x%X\n", ret);
        return ret;
    }
    void write_signal(unsigned address, unsigned datum, unsigned offset) {fast_write(address, datum); }

    // This is the main process to simulate the IP behavior
    void prc_ESDHCV2();

    // -- External signals
    SC_HAS_PROCESS( ESDHCV2_module );
    ESDHCV2_module(sc_module_name name_, tzic_module &tzic_);

    void connect_card(sd_card & card);

private:

    void reset_DAT_line();

/*This function handles every type of outgoing ESDHC interruption.  It
 * is responsable for checking if that kind of interruption
 * can be asserted and if so, sets IRQSTAT, and service_interrupt()
 * as necessary.
 * It is controlled by IRQSTATEN and IRQSIGEN
 */
    void generate_signal(enum irqstat irqnum);
    void execute_xfertyp_command();
    void update_state(const enum state new_state);

    /* This method decodes a sd_response and distribute the response code
     * among CMD_RESP registers.  */
    void decode_response(struct sd_response response);

    void do_reset(bool hard_reset=true)
    {
        // Initial values
        regs[DSADR/4]   = 0x0;
        regs[BLKATTR/4] = 0x0;
        regs[CMDARG/4]  = 0x0;

        //XFERTYP
        DMAEN  = false;
        BCEN   = false;
        AC12EN = false;
        DTDSEL = false;
        MSBSEL = false;
        RSPTYP = 0;
        CCCEN  = false;
        CICEN  = false;
        DPSEL  = false;
        CMDTYP = 0;
        CMDINX = 0;
        //--

        regs[CMDRSP0/4] = 0;
        regs[CMDRSP1/4] = 0;
        regs[CMDRSP2/4] = 0;
        regs[CMDRSP3/4] = 0;
        regs[DATPORT/4] = 0;

        //Present State = 0x0
        DLSL   = 0;
        CLSL   = false;
        WPSPL  = false;
        CDPL   = false;
        CINS   = false;
        BREN   = false;
        BWEN   = false;
        RTA    = false;
        WTA    = false;
        SDOFF  = false;
        PEROFF = false;
        HCKOFF = false;
        IPGOFF = false;
        SDSTB  = false;
        DLA    = false;
        CDIHB  = false;
        CIHB   = false;
        //--

        //Protocol control
        WECRM   = false;
        WECINS  = false;
        WECINT  = false;
        IABG    = false;
        RWCTL   = false;
        CREQ    = false;
        SABGREQ = false;
        DMAS[2] = false;
        CDSS    = false;
        CDTL    = false;
        EMODE[2] = false;
        D3CD    = false;
        DTW[2]  = false;
        LCTL    = false;
        //--

        //System Control 0x8008
        INITA   = false;
        RSTD    = false;
        RSTC    = false;
        RSTA    = false;
        DTOCV   = false;
        SDCLKFS = 0x08;
        DVS     = false;
        SDCLKEN = true;
        PEREN   = false;
        HCKEN   = false;
        IPGEN   = false;
        //--
        regs[WML/4] = 0x08100810;
        WR_BRST_LEN = 0x8;
        WR_WML      = 0x10;
        RD_BRST_LEN = 0x8;
        RD_WML      = 0x10;

        regs[IRQSTAT/4]  = 0x0;
        regs[IRQSTATEN/4]  = 0x117F013F;
        regs[IRQSIGEN/4]   = 0x0;
        regs[AUTOC12ERR/4] = 0x0;
        regs[HOSTCAPBLT/4] = 0x07F30000;
        regs[ADMAES/4]     = 0x0;
        regs[ADSADDR/4]    = 0x0;
        regs[VENDOR/4]     = 0x1;
        regs[MMCBOOT/4]     = 0x0;
        regs[HOSTVER/4]    = 0x00001201;
        //--
    }
};

#endif /* !ESDHCV2_H.  */
