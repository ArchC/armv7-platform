#include "esdhcv2.h"
#include "arm_interrupts.h"

extern bool DEBUG_ESDHCV2;
#define dprintf(args...) if(DEBUG_ESDHCV2){fprintf(stderr,args);}

#define isBitSet(variable, position) (((variable & (1 << (position))) != 0) ? true : false)
#define setBit(variable, position) variable = variable | (1 << (position))


ESDHCV2_module::ESDHCV2_module (sc_module_name name_, tzic_module &tzic_,
                                uint32_t start_add, uint32_t end_add):
    sc_module(name_), peripheral(start_add, end_add), tzic(tzic_)
{
    do_reset();

    // A SystemC thread never finishes execution, but transfers control back
    // to SystemC kernel via wait() calls.
    SC_THREAD(prc_ESDHCV2);


}

ESDHCV2_module::~ESDHCV2_module() {
}


void ESDHCV2_module::prc_ESDHCV2() {
    dprintf("-------------------- ESDHCV2 -------------------- \n");
}

unsigned ESDHCV2_module::fast_read(unsigned address) {
    switch(address)
    {
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
    default:
        return regs[address/4];
    }
}

void ESDHCV2_module::fast_write(unsigned address, unsigned datum) {
    switch(address)
    {
    case DSADR:
        regs[DSADR/4]   = datum & ~(0b11);
        break;
    case BLKATTR:
        regs[BLKATTR/4] = datum & ~((0b111)<< 13);
        break;
    case CMDARG:
        if(!CIHB){
            regs[CMDARG/4] = datum;
        }
        break;
    case XFERTYP:
        //Can write only if no CIHB and CDIHB or if CDIB, there is no data
        if( !((CDIHB && isBitSet(datum, 21))  || CIHB) )
        { //if CDIHB
            if(isBitSet(datum, 1) ||
               (~isBitSet(datum,5) && (regs[BLKATTR/4] & 0xFFF) != 0))
            {
                if(!(isBitSet(datum, 4) && !WPSPL))
                {
                    if(!data_transfer)
                    {
                        DPSEL  = isBitSet(datum,21);
                        MSBSEL = isBitSet(datum, 5);
                        DTDSEL = isBitSet(datum, 4);
                        AC12EN = isBitSet(datum, 2);
                        BCEN   = isBitSet(datum, 1);
                        DMAEN  = isBitSet(datum, 0);
                    }
                    CMDINX = (datum & (0b111111<<24)>> 24);
                    RSPTYP = (datum & (0b11<<16) >> 16);
                    CMDTYP = (datum & (0b11<<22) >> 22);
                }
            }
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
        CDTL   = isBitSet(datum, 6);
        EMODE[1]= isBitSet(datum, 5);
        EMODE[0]= isBitSet(datum, 4);
        D3CD    = isBitSet(datum, 3);
        DTW[1]  = isBitSet(datum, 2);
        DTW[0]  = isBitSet(datum, 1);
        LCTL    = isBitSet(datum, 0);
        break;
    case SYSCTL:
        INITA  = isBitSet(datum,27);
        RSTD   = isBitSet(datum,26);
        RSTC   = isBitSet(datum,25);
        RSTA   = isBitSet(datum, 24);
        DTOCV  = (datum >> 16) & 0b1111;
        SDCLKFS = (datum >> 8) & 0xFF;
        DVS    = (datum >> 4)  & 0x1111;
        SDCLKEN = isBitSet(datum,3);
        PEREN = isBitSet(datum, 2);
        HCKEN = isBitSet(datum, 1);
        IPGEN = isBitSet(datum, 0);
    case IRQSTAT:
        datum = datum & 0x117F01FE;
        regs[IRQSTAT/4] &= ~datum;
        break;
    case IRQSTATEN:
        regs[IRQSTATEN/4] = (1<<28) | (datum & 0x17F01FE);
        break;
    case IRQSIGEN:
        regs[IRQSTATEN/4] = (datum & 0x117F01FE);
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
        
    default:
        break; //ignore write
    }
}

