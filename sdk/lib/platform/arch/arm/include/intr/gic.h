#ifndef   __GIC_H__
#define   __GIC_H__

#include "common.h"
//#include <platform/arch/arm/include/mx6q/MCIMX6Y2.h>

//! @brief Offsets to the GIC registers.
enum _gic_base_offsets
{
    kGICDBaseOffset = 0x1000,   //!< GIC distributor offset.
#if defined(CHIP_MX6UL)
    kGICCBaseOffset = 0x2000     //!< GIC CPU interface offset.
#else    
    kGICCBaseOffset = 0x100     //!< GIC CPU interface offset.
#endif    
};


typedef struct
{
        //uint32_t RESERVED0[1024];
	uint32_t RESERVED0_1_0[64];
	uint32_t testRsv0;
	uint32_t testRsv1;
	uint32_t testRsv2;
	uint32_t iIrqVec;
	uint32_t iEOIR;
	uint32_t testRsv3;
	uint32_t testRsv4;
	uint32_t testRsv5;
	uint32_t RESERVED0_1_1[24];
	uint32_t RESERVED0_1_2[32];
	uint32_t RESERVED0_1_3[128];
	uint32_t RESERVED0_2[256];
	uint32_t RESERVED0_3[256];
	uint32_t RESERVED0_4[256];
  __IOM uint32_t D_CTLR;                 /*!< Offset: 0x1000 (R/W) Distributor Control Register */
  __IM  uint32_t D_TYPER;                /*!< Offset: 0x1004 (R/ )  Interrupt Controller Type Register */
  __IM  uint32_t D_IIDR;                 /*!< Offset: 0x1008 (R/ )  Distributor Implementer Identification Register */
        uint32_t RESERVED1[29];
  __IOM uint32_t D_IGROUPR[16];          /*!< Offset: 0x1080 - 0x0BC (R/W) Interrupt Group Registers */
        uint32_t RESERVED2[16];
  __IOM uint32_t D_ISENABLER[16];        /*!< Offset: 0x1100 - 0x13C (R/W) Interrupt Set-Enable Registers */
        uint32_t RESERVED3[16];
  __IOM uint32_t D_ICENABLER[16];        /*!< Offset: 0x1180 - 0x1BC (R/W) Interrupt Clear-Enable Registers */
        uint32_t RESERVED4[16];
  __IOM uint32_t D_ISPENDR[16];          /*!< Offset: 0x1200 - 0x23C (R/W) Interrupt Set-Pending Registers */
        uint32_t RESERVED5[16];
  __IOM uint32_t D_ICPENDR[16];          /*!< Offset: 0x1280 - 0x2BC (R/W) Interrupt Clear-Pending Registers */
        uint32_t RESERVED6[16];
  __IOM uint32_t D_ISACTIVER[16];        /*!< Offset: 0x1300 - 0x33C (R/W) Interrupt Set-Active Registers */
        uint32_t RESERVED7[16];
  __IOM uint32_t D_ICACTIVER[16];        /*!< Offset: 0x1380 - 0x3BC (R/W) Interrupt Clear-Active Registers */
        uint32_t RESERVED8[16];
  __IOM uint8_t  D_IPRIORITYR[512];      /*!< Offset: 0x1400 - 0x5FC (R/W) Interrupt Priority Registers */
        uint32_t RESERVED9[128];
  __IOM uint8_t  D_ITARGETSR[512];       /*!< Offset: 0x1800 - 0x9FC (R/W) Interrupt Targets Registers */
        uint32_t RESERVED10[128];
  __IOM uint32_t D_ICFGR[32];            /*!< Offset: 0x1C00 - 0xC7C (R/W) Interrupt configuration registers */
        uint32_t RESERVED11[32];
  __IM  uint32_t D_PPISR;                /*!< Offset: 0x1D00 (R/ ) Private Peripheral Interrupt Status Register */
  __IM  uint32_t D_SPISR[15];            /*!< Offset: 0x1D04 - 0xD3C (R/ ) Shared Peripheral Interrupt Status Registers */
        uint32_t RESERVED12[112];
  __OM  uint32_t D_SGIR;                 /*!< Offset: 0x1F00 ( /W) Software Generated Interrupt Register */
        uint32_t RESERVED13[3];
  __IOM uint8_t  D_CPENDSGIR[16];        /*!< Offset: 0x1F10 - 0xF1C (R/W) SGI Clear-Pending Registers */
  __IOM uint8_t  D_SPENDSGIR[16];        /*!< Offset: 0x1F20 - 0xF2C (R/W) SGI Set-Pending Registers */
        uint32_t RESERVED14[40];
  __IM  uint32_t D_PIDR4;                /*!< Offset: 0x1FD0 (R/ ) Peripheral ID4 Register */
  __IM  uint32_t D_PIDR5;                /*!< Offset: 0x1FD4 (R/ ) Peripheral ID5 Register */
  __IM  uint32_t D_PIDR6;                /*!< Offset: 0x1FD8 (R/ ) Peripheral ID6 Register */
  __IM  uint32_t D_PIDR7;                /*!< Offset: 0x1FDC (R/ ) Peripheral ID7 Register */
  __IM  uint32_t D_PIDR0;                /*!< Offset: 0x1FE0 (R/ ) Peripheral ID0 Register */
  __IM  uint32_t D_PIDR1;                /*!< Offset: 0x1FE4 (R/ ) Peripheral ID1 Register */
  __IM  uint32_t D_PIDR2;                /*!< Offset: 0x1FE8 (R/ ) Peripheral ID2 Register */
  __IM  uint32_t D_PIDR3;                /*!< Offset: 0x1FEC (R/ ) Peripheral ID3 Register */
  __IM  uint32_t D_CIDR0;                /*!< Offset: 0x1FF0 (R/ ) Component ID0 Register */
  __IM  uint32_t D_CIDR1;                /*!< Offset: 0x1FF4 (R/ ) Component ID1 Register */
  __IM  uint32_t D_CIDR2;                /*!< Offset: 0x1FF8 (R/ ) Component ID2 Register */
  __IM  uint32_t D_CIDR3;                /*!< Offset: 0x1FFC (R/ ) Component ID3 Register */

  __IOM uint32_t C_CTLR;                 /*!< Offset: 0x2000 (R/W) CPU Interface Control Register */
  __IOM uint32_t C_PMR;                  /*!< Offset: 0x2004 (R/W) Interrupt Priority Mask Register */
  __IOM uint32_t C_BPR;                  /*!< Offset: 0x2008 (R/W) Binary Point Register */
  __IM  uint32_t C_IAR;                  /*!< Offset: 0x200C (R/ ) Interrupt Acknowledge Register */
  __OM  uint32_t C_EOIR;                 /*!< Offset: 0x2010 ( /W) End Of Interrupt Register */
  __IM  uint32_t C_RPR;                  /*!< Offset: 0x2014 (R/ ) Running Priority Register */
  __IM  uint32_t C_HPPIR;                /*!< Offset: 0x2018 (R/ ) Highest Priority Pending Interrupt Register */
  __IOM uint32_t C_ABPR;                 /*!< Offset: 0x201C (R/W) Aliased Binary Point Register */
  __IM  uint32_t C_AIAR;                 /*!< Offset: 0x2020 (R/ ) Aliased Interrupt Acknowledge Register */
  __OM  uint32_t C_AEOIR;                /*!< Offset: 0x2024 ( /W) Aliased End Of Interrupt Register */
  __IM  uint32_t C_AHPPIR;               /*!< Offset: 0x2028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
        uint32_t RESERVED15[41];
  __IOM uint32_t C_APR0;                 /*!< Offset: 0x20D0 (R/W) Active Priority Register */
        uint32_t RESERVED16[3];
  __IOM uint32_t C_NSAPR0;               /*!< Offset: 0x20E0 (R/W) Non-secure Active Priority Register */
        uint32_t RESERVED17[6];
  __IM  uint32_t C_IIDR;                 /*!< Offset: 0x20FC (R/ ) CPU Interface Identification Register */
        uint32_t RESERVED18[960];
  __OM  uint32_t C_DIR;                  /*!< Offset: 0x3000 ( /W) Deactivate Interrupt Register */
} GIC_V2_Type;


typedef struct {
  __IO uint32_t CR;                                /**< GPT Control Register, offset: 0x0 */
  __IO uint32_t PR;                                /**< GPT Prescaler Register, offset: 0x4 */
  __IO uint32_t SR;                                /**< GPT Status Register, offset: 0x8 */
  __IO uint32_t IR;                                /**< GPT Interrupt Register, offset: 0xC */
  __IO uint32_t OCR[3];                            /**< GPT Output Compare Register 1..GPT Output Compare Register 3, array offset: 0x10, array step: 0x4 */
  __I  uint32_t ICR[2];                            /**< GPT Input Capture Register 1..GPT Input Capture Register 2, array offset: 0x1C, array step: 0x4 */
  __I  uint32_t CNT;                               /**< GPT Counter Register, offset: 0x24 */
} GPT_V2_Type;


void gic_init(void);
void gic_enable_irq(uint32_t nr);
void gic_disable_irq(uint32_t nr);


typedef void (*irq_handler_t) (uint32_t nr, void *param);

void request_irq(uint32_t irq, irq_handler_t handler, void *param);
void system_init_irq_table(void);

/**
 * @brief IRQ handle for specific IRQ
 */
typedef struct _sys_irq_handle
{
    irq_handler_t irq_handler; /**< IRQ handler for specific IRQ */
    //	void *param;                 /**< User param for handler callback */
} sys_irq_handle_t;


//! @brief GIC CPU interface registers.
//!
//! Uses the GICv2 register names. Does not include GICv2 registers.
struct _gicd_registers
{
    uint32_t CTLR;              //!< Distributor Control Register.
    uint32_t TYPER;             //!< Interrupt Controller Type Register.
    uint32_t IIDR;              //!< Distributor Implementer Identification Register.
    uint32_t _reserved0[29];
    uint32_t IGROUPRn[8];       //!< Interrupt Group Registers.
    uint32_t _reserved1[24];
    uint32_t ISENABLERn[32];    //!< Interrupt Set-Enable Registers.
    uint32_t ICENABLERn[32];    //!< Interrupt Clear-Enable Registers.
    uint32_t ISPENDRn[32];      //!< Interrupt Set-Pending Registers.
    uint32_t ICPENDRn[32];      //!< Interrupt Clear-Pending Registers.
    uint32_t ICDABRn[32];       //!< Active Bit Registers.
    uint32_t _reserved2[32];
    uint8_t IPRIORITYRn[255 * sizeof(uint32_t)];  //!< Interrupt Priority Registers. (Byte accessible)
    uint32_t _reserved3;
    uint8_t ITARGETSRn[255 * sizeof(uint32_t)];   //!< Interrupt Processor Targets Registers. (Byte accessible)
    uint32_t _reserved4;
    uint32_t ICFGRn[64];        //!< Interrupt Configuration Registers.
    uint32_t _reserved5[128];
    uint32_t SGIR;              //!< Software Generated Interrupt Register
};

//! @brier Type for the GIC distributor registers.
typedef volatile struct _gicd_registers gicd_t;

//! @brief GIC CPU interface registers.
//!
//! Uses the GICv2 register names. Does not include GICv2 registers.
struct _gicc_registers
{
    uint32_t CTLR;  //!< CPU Interface Control Register.
    uint32_t PMR;   //!< Interrupt Priority Mask Register.
    uint32_t BPR;   //!< Binary Point Register.
    uint32_t IAR;   //!< Interrupt Acknowledge Register.
    uint32_t EOIR;  //!< End of Interrupt Register.
    uint32_t RPR;   //!< Running Priority Register.
    uint32_t HPPIR; //!< Highest Priority Pending Interrupt Register.
    uint32_t ABPR;  //!< Aliased Binary Point Register. (only visible with a secure access)
    uint32_t _reserved[56];
    uint32_t IIDR;  //!< CPU Interface Identification Register.
};

//! @brier Type for the GIC CPU interface registers.
typedef volatile struct _gicc_registers gicc_t;



/*
 * -----------------------------------------------------------------------
 * Interrupt Distributor registers
 * -----------------------------------------------------------------------
 */

#define	ARM_GICD_SIZE			0x1000

#define	ARM_GICD_CTLR			0x000
#define	ARM_GICD_TYPER			0x004
#define ARM_GICD_IIDR			0x008
#define ARM_GICD_IGROUPRn		0x080
#define	ARM_GICD_ISENABLERn		0x100
#define	ARM_GICD_ICENABLERn		0x180
#define	ARM_GICD_ISPENDRn		0x200
#define	ARM_GICD_ICPENDRn		0x280
#define	ARM_GICD_ISACTIVERn		0x300
#define	ARM_GICD_ICACTIVERn		0x300
#define	ARM_GICD_IPRIORITYn		0x400
#define	ARM_GICD_ITARGETSRn		0x800
#define	ARM_GICD_ICFGRn			0xc00
#define	ARM_GICD_SGIR			0xf00
#define ARM_GICD_CPENDSGIRn		0xf10
#define ARM_GICD_SPENDSGIRn		0xf20
#define ARM_GICD_PIDR4			0xfd0
#define ARM_GICD_PIDR5			0xfd4
#define ARM_GICD_PIDR6			0xfd8
#define ARM_GICD_PIDR7			0xfdc
#define ARM_GICD_PIDR0			0xfe0
#define ARM_GICD_PIDR1			0xfe4
#define ARM_GICD_PIDR2			0xfe8
#define ARM_GICD_PIDR3			0xfec
#define ARM_GICD_CIDR0			0xff0
#define ARM_GICD_CIDR1			0xff4
#define ARM_GICD_CIDR2			0xff8
#define ARM_GICD_CIDR3			0xffc

#define ARM_GICD_TYPER_ITLN		0x1f
#define	ARM_GICD_CTLR_EN		0x00000001

/*
 * -----------------------------------------------------------------------
 * CPU interface registers
 * -----------------------------------------------------------------------
 */

#define ARM_GICC_SIZE		0x2000

#define	ARM_GICC_CTLR		0x00
#define	ARM_GICC_PMR		0x04
#define	ARM_GICC_BBPR		0x08
#define	ARM_GICC_IAR		0x0c
#define	ARM_GICC_EOIR		0x10
#define	ARM_GICC_RPR		0x14
#define	ARM_GICC_HPPIR		0x18
#define	ARM_GICC_ABPR		0x1c
#define	ARM_GICC_AIAR		0x20
#define	ARM_GICC_AEOIR		0x24
#define	ARM_GICC_AHPPIR		0x28
#define	ARM_GICC_APR0		0xd0
#define	ARM_GICC_NSAPR0		0xe0
#define	ARM_GICC_IIDR		0xfc
#define	ARM_GICC_DIR		0x18

/*
 * CPU_CTRL bits
 */
#define	ARM_GICC_CTLR_EN		0x00000001

/*
 * CPU_IACK bits
 */
#define	ARM_GICC_IAR_SRCID	0x00001c00



#endif

