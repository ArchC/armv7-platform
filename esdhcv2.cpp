#include "esdhcv2.h"
#include "arm_interrupts.h"

extern bool DEBUG_ESDHCV2;
#define dprintf(args...) \
       if (DEBUG_ESDHCV2) \
           fprintf(stderr,args); \

#define isBitSet(reg, bit) (((reg & (1 << (bit))) != 0) ? true : false)
#define setBit(reg, bit) (reg = regs[reg/4] | (1 << (bit)))

void ESDHCV2_module::reset_DAT_line()
{
  /* Performs a DAT line reset, erasing any remains of a data transfers.
   Might be activated by high RSTD bit assertion.  */
    BREN = false;
    BWEN = false;
    RTA  = false;
    WTA  = false;
    DLA  = false;
    CDIHB = false;
    CREQ = false;
    SABGREQ = false;

    // Clear IRQSTAT Bits:BRR, BWR, DINT, BGE, TC
    regs[IRQSTAT/4] = regs[IRQSTAT/4] & ~0x3E;

    //Clear data port
    while(ibuffer.size() > 0)
        ibuffer.pop();
}

void ESDHCV2_module::connect_card(sd_card & card)
{
    port = &card;
    CINS=true;  // Flag to host a card is inserted
}

ESDHCV2_module::ESDHCV2_module(sc_module_name name_, tzic_module &tzic_):
    sc_module(name_), tzic(tzic_)
{
    do_reset();
    CIHB = false; //Ready to receive first command!

    current_state = IDLE;

    // A SystemC thread never finishes execution, but transfers control back
    // to SystemC kernel via wait() calls
    SC_THREAD(prc_ESDHCV2);
}

void ESDHCV2_module::prc_ESDHCV2()
{
    do{
      //        dprintf("-------------------- ESDHCV2 -------------------- \n");
        wait(1, SC_NS);

        if(current_state == IDLE)
            continue;

        // Handle SD connection.
        sd_protocol();

        // Host Protocol
        host_protocol();

    }while(1);
}

void ESDHCV2_module::sd_protocol()
{
    if(current_state == HOST_READ_TRANSFER)
    {
        // If data ready on the bus.
        if(port->data_line_busy == true)
        {
            // Read next block.
            port->read_dataline(ibuffer, BLKSIZE);

            // Multiple block transfer.
            if (MSBSEL == true)
            {
                //If necessary reduce BlockCount and issue AUTOCMD12
                if(BCEN == true)
                {
                    BLKCNT = BLKCNT - 1;

                    if(BLKCNT == 0 && AC12EN == true)
                    {
                        // Issue AUTOCMD12.
                        port->exec_cmd(12, 0b11, regs[CMDARG/4]);

                        BLKCNT = BLKCNT_BKP;
                        update_state(HOST_READ_DONE);
                    }
                }
            }
            else
            {
                update_state(HOST_READ_DONE);
            }
        }
    }
    else if(current_state == HOST_WRITE)
    {
        printf("ESDHC WRITE not implement in this model");
        exit(1);
    }
}
void ESDHCV2_module::host_protocol()
{
    if(current_state == HOST_READ_TRANSFER
       || current_state == HOST_READ_DONE)
    {
        // Indicate there is valid data available.
        if(ibuffer.size() >= RD_WML)
        {
            BREN = true;
            generate_signal(IRQ_BRR);
        }

        // Verify operation complete.
        if(ibuffer.size() == 0 && current_state == HOST_READ_DONE)
        {
            generate_signal(IRQ_TC);
            update_state(IDLE);
        }
    }
}

void ESDHCV2_module::execute_xfertyp_command()
{
    //Host driver sent a new command to XFERTYP. We must execute it and
    //recover reponses to correct registers.
    sd_response resp = port->exec_cmd(CMDINX, CMDTYP, regs[CMDARG/4]);
    decode_response(resp);// USE RESP_TYP bit!!!

    if(CICEN == true || CCCEN == true)
    {
        dprintf("%s: CRC & Index check not implemented in this model.\n",
                this->name());
    }

    //Command issued requires data transfer
    if(DPSEL == true)
    {
        // Block further data transfer commands.
        CDIHB = true;

        if(DTDSEL == true)
            update_state(HOST_READ_TRANSFER);
        else
            update_state(HOST_WRITE);
    }

    // Signal command executed.
    generate_signal(IRQ_CC);

    // Ready to execute next command.
    CIHB = false;
}

void ESDHCV2_module::generate_signal(const enum irqstat irqnum)
{
    uint32_t irq = (1<<irqnum);

    if(regs[IRQSTATEN/4] & irq)
    {
        printf("esdhc: Generating IRQ signal %d\n", irqnum);
        //Can assert IRQSTAT
        regs[IRQSTAT/4] = regs[IRQSTAT/4] | irq;
    }
    else
        printf ("esdhc: IRQ signal %d, will not be generated "
                 " due to IRQSTATEN flags", irqnum);

    if(regs[IRQSIGEN/4] & irq)
    {
        dprintf("esdhc: Generating IRQ interrupt due to irq signal %d\n",
                irqnum);

        //Can generate interruption
        tzic.interrupt(ESDHCV2_1_IRQ, /*deassert=*/true);
    }
}

void ESDHCV2_module::update_state(const enum state new_state)
{
    // Signal DLA will be used.
    DLA = true;

    if(new_state == HOST_READ_TRANSFER)
    {
        // Signal DLA will be used.
        DLA = true;

        // Signal direction as READ.
        RTA = true;

        // Prevent from receiving further data transfer commands.
        CDIHB = true;

        current_state = HOST_READ_TRANSFER;
    }
    else if (new_state == HOST_READ_DONE)
    {
        DLA = false;
        RTA = false;
        CDIHB = false;
        current_state = HOST_READ_DONE;
    }
    else if(new_state == HOST_WRITE)
    {
        printf("ESDHC WRITE not implement in this model");
        exit(1);
    }
    else if(new_state == IDLE)
    {
        current_state = IDLE;
    }
    else
    {
        printf("%s: State %d, not implemented!", new_state);
    }
}

void ESDHCV2_module::decode_response(struct sd_response resp)
{
    switch (resp.type)
    {
    case R1:
    case R1b:
    case R3:
    case R4:
    case R5:
    case R5b:
    case R7:
        // Response[0] is ignored
        regs[CMDRSP0/4] = ((resp.response[1] << 24) |
                           (resp.response[2] << 16) |
                           (resp.response[3] << 8)  |
                           (resp.response[4] << 0));
        regs[CMDRSP1/4] = 0;
        regs[CMDRSP2/4] = 0;
        regs[CMDRSP3/4] = 0;
        break;

    case R6:
        // Response[0] is ignored
        regs[CMDRSP0/4] = ((resp.response[1] << 24) |
                           (resp.response[2] << 16) |
                           (resp.response[3] << 8)  |
                           (resp.response[4] << 0) &
                           ~0b1); //R[39:9] Eliminate first bit
        regs[CMDRSP1/4] = 0;
        regs[CMDRSP2/4] = 0;
        regs[CMDRSP3/4] = 0;
        break;

    case R1bCMD12:
        // Response[0] is ignored
        regs[CMDRSP0/4] = 0;
        regs[CMDRSP1/4] = 0;
        regs[CMDRSP2/4] = 0;
        regs[CMDRSP3/4] = ((resp.response[1] << 24) |
                           (resp.response[2] << 16) |
                           (resp.response[3] << 8)  |
                           (resp.response[4] << 0));
        break;
    case R2:
        regs[CMDRSP0/4] = ((resp.response[1] << 24) |
                           (resp.response[2] << 16) |
                           (resp.response[3] << 8)  |
                           (resp.response[4] << 0));

        regs[CMDRSP1/4] = ((resp.response[5] << 24) |
                           (resp.response[6] << 16) |
                           (resp.response[7] << 8)  |
                           (resp.response[8] << 0));

        regs[CMDRSP2/4] = ((resp.response[9] << 24) |
                           (resp.response[10] << 16) |
                           (resp.response[11] << 8)  |
                           (resp.response[12] << 0));

        regs[CMDRSP3/4] = ((resp.response[13] << 16) |
                           (resp.response[14] << 8)  |
                           (resp.response[15] << 0));
        break;
    default:
        printf("SD_CARD: Decode for response type %d not implemented\n",
               resp.type);
    }
}

unsigned ESDHCV2_module::fast_read(unsigned address)
{
    dprintf("ESDHCv2 READ address: register: 0x%X ", address);
    uint32_t datum;
    switch(address)
    {
    case BLKATTR:
        return
            ((BLKCNT << 16) |
             (BLKSIZE & 0x1fff));
    case XFERTYP:
        return
            (((CMDINX & 0b111111) << 24) |
             ((CMDTYP & 0b0000011) << 22) |
             (DPSEL  << 21) |
             (CICEN  << 20) |
             (CCCEN  << 19) |
             ((RSPTYP & 0b0000011) << 16) |
             (MSBSEL << 5)  |
             (DTDSEL << 4)  |
             (AC12EN << 2)  |
             (BCEN   << 1)  |
             (DMAEN  << 0));
    case PRSSTAT:
        return
            (((DLSL & 0b1111111) << 24) |
             (CLSL   << 23)|
             (WPSPL  << 19)|
             (CDPL   << 18)|
             (CINS   << 16)|
             (BREN   << 11)|
             (BWEN   << 10)|
             (RTA    << 9)|
             (WTA    << 8)|
             (SDOFF  << 7)|
             (PEROFF << 6)|
             (HCKOFF << 5)|
             (IPGOFF << 4)|
             (SDSTB  << 3)|
             (DLA    << 2)|
             (CDIHB  << 1)|
             (CIHB   << 0));
    case PROCTL:
        return
            ((WECRM   << 26)|
             (WECINS  << 25)|
             (WECINT  << 24)|
             (IABG    << 19)|
             (RWCTL   << 18)|
             (CREQ    << 17)|
             (SABGREQ << 16)|
             (DMAS[1] << 9) |
             (DMAS[0] << 8) |
             (CDSS    << 7) |
             (CDTL    << 6) |
             (EMODE[1]<< 5) |
             (EMODE[0]<< 4) |
             (D3CD    << 3) |
             (DTW[1]  << 2) |
             (DTW[0]  << 1) |
             (LCTL    << 0));
    case SYSCTL:
        return
            ((INITA << 27) |
             ((DTOCV & 0b1111) << 16)|
             (SDCLKFS << 8)|
             ((DVS   & 0b1111) <<  4)|
             (SDCLKEN << 3)|
             (PEREN   << 2)|
             (HCKEN   << 1)|
             (IPGEN   << 0));
    case WML:
        return
            ((WR_BRST_LEN << 24)|
             (WR_WML      << 16)|
             (RD_BRST_LEN << 8) |
             (RD_WML      << 0));
    case FEVT:
        return 0;
    case ADMAEST:
        return
            ((ADMADCE << 3) |
             (ADMALME << 2) |
             (ADMAES  & 0b11));
        break;
    case DATPORT:
        BREN = false;
        datum = 0;
        for(int i=0; i < 4 && ibuffer.empty() == false; i++)
        {
            datum |= (ibuffer.front() << (8*i));
            ibuffer.pop();
        }
        dprintf("DATAPORT: 0x%X\n", datum);
        return datum;
        break;
    case IRQSTAT:
        return regs[IRQSTAT/4];
        break;
    default:
        return regs[address/4];
    }
}

void ESDHCV2_module::fast_write(unsigned address, unsigned datum)
{
    dprintf("ESDHCv2 WRITE address: 0x%X data:0x%X\n", address, datum);
    switch(address)
    {
    case DSADR:
        regs[DSADR/4]   = datum & ~(0b11);
        break;
    case CMDARG:
        if(!CIHB)
            regs[CMDARG/4] = datum;
        break;

    case XFERTYP:
        if(!CIHB)
        {
            DPSEL  = isBitSet(datum,21);
            MSBSEL = isBitSet(datum, 5);
            DTDSEL = isBitSet(datum, 4);
            AC12EN = isBitSet(datum, 2);
            BCEN   = isBitSet(datum, 1);
            DMAEN  = isBitSet(datum, 0);

            CMDINX = ((datum & (0b111111<<24))>> 24);
            RSPTYP = ((datum & (0b11<<16)) >> 16);
            CMDTYP = ((datum & (0b11<<22)) >> 22);

            //  Prevent further commands to be sent before this one is
            // processed
            CIHB = true;
            execute_xfertyp_command();
        }
        break;

    case PROCTL:
        WECRM   = isBitSet(datum, 26);
        WECINS  = isBitSet(datum, 25);
        WECINT  = isBitSet(datum, 24);
        IABG    = isBitSet(datum, 19);
        RWCTL   = isBitSet(datum, 18);
        CREQ    = isBitSet(datum, 17);
        SABGREQ = isBitSet(datum, 16);
        DMAS[1] = isBitSet(datum, 9);
        DMAS[0] = isBitSet(datum, 8);
        CDSS    = isBitSet(datum, 7);
        CDTL    = isBitSet(datum, 6);
        EMODE[1]= isBitSet(datum, 5);
        EMODE[0]= isBitSet(datum, 4);
        D3CD    = isBitSet(datum, 3);
        /* Data width is ignored.  */
        DTW[1]  = isBitSet(datum, 2);
        DTW[0]  = isBitSet(datum, 1);
        LCTL    = isBitSet(datum, 0);
        break;

    case SYSCTL:
        INITA   = isBitSet(datum,27);
        RSTD    = isBitSet(datum,26);
        RSTC    = isBitSet(datum,25);
        RSTA    = isBitSet(datum, 24);
        DTOCV   = (datum >> 16) & 0b1111;
        SDCLKFS = (datum >> 8) & 0xFF;
        DVS     = (datum >> 4)  & 0x1111;
        SDCLKEN = isBitSet(datum,3);
        PEREN   = isBitSet(datum, 2);
        HCKEN   = isBitSet(datum, 1);
        IPGEN   = isBitSet(datum, 0);

        // Fake a Initialization.
        if(INITA)
            INITA = false;

        // Software reset.
        if(RSTA)
            do_reset(false);

        //Signals that clock is stable.
        SDSTB = true;
        break;

    case IRQSTAT:  //Write 1 to clear register
      regs[IRQSTAT/4] = regs[IRQSTAT/4] & ~datum;
      break;

    case IRQSTATEN:
        regs[IRQSTATEN/4] = (1<<28) | (datum & 0x17F01FF);
        break;

    case IRQSIGEN:
        regs[IRQSIGEN/4] = (datum & 0x117F01FE);
        break;

    case WML:
        WR_BRST_LEN = (datum >> 24) & 0b11111;
        WR_WML      = (datum >> 16) & 0xFF;
        RD_BRST_LEN = (datum >>  8) & 0b11111;
        RD_WML      = (datum & 0xFF);
        break;

    case FEVT:
        regs[IRQSTAT/4] = regs[IRQSTATEN/4] & datum;
        break;

    case ADSADDR:
        regs[ADSADDR/4] = datum & 0xFE;
        break;

    case VENDOR:
        regs[VENDOR/4]  = datum & 0xFFF0003;
        break;

    case MMCBOOT:
        regs[MMCBOOT/4]  = datum & 0xFF0F;

    case BLKATTR:
        BLKCNT = datum >> 16;
        BLKCNT_BKP = BLKCNT;
        BLKSIZE = datum & 0x1FF;
        break;

    default:
        dprintf("ignored");
        break; //ignore write
    }
    dprintf("\n");
}

