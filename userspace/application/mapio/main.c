#include <libc/include/string.h>
#include <platform/include/platform.h>

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/* GPIO - Register Offset */
#define GPIO_DR                                 0x00
#define GPIO_GDIR                               0x04
#define GPIO_PSR                                0x08
#define GPIO_ICR1                               0x0C
#define GPIO_ICR2                               0x10
#define GPIO_IMR                                0x14
#define GPIO_ISR                                0x18
#define GPIO_EDGE_SEL                           0x1C

/* GPIO - Register Size */
#define GPIO_REG_SIZE							0x20


/* GPIO - Peripheral instance base addresses */
/** Peripheral GPIO1 base address */
#define GPIO1_BASE                               (0x209C000u)
/** Peripheral GPIO2 base address */
#define GPIO2_BASE                               (0x20A0000u)
/** Peripheral GPIO3 base address */
#define GPIO3_BASE                               (0x20A4000u)
/** Peripheral GPIO4 base address */
#define GPIO4_BASE                               (0x20A8000u)
/** Peripheral GPIO5 base address */
#define GPIO5_BASE                               (0x20AC000u)
/** Peripheral GPIO6 base address */
#define GPIO6_BASE                               (0x20B0000u)
/** Peripheral GPIO7 base address */
#define GPIO7_BASE                               (0x20B4000u)

#define GPIO_BANK_NUM                            0x07

#define IOMUXC_BASE                             (0x020E0000u)
#define IOMUXC_SIZE                             (0x950u)


#ifndef USER_LED_PORT
 #define USER_LED_PORT                          (1) /* GPIO2 */
#endif
#ifndef USER_LED_FLICKS
 #define USER_LED_FLICKS                        (20)
#endif
#ifndef USER_LED_PERIOD
 #define USER_LED_PERIOD                        (200)
#endif

/*==================================================================================================
*                                      DATA
==================================================================================================*/
static const paddr_t gpio_base[] = {
    GPIO1_BASE, GPIO2_BASE, GPIO3_BASE, GPIO4_BASE, GPIO5_BASE, GPIO6_BASE, GPIO7_BASE
};

static const uint16_t IOMUX_MUX_RegTab[7][32] = 
{   
/*1*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x220, 0x224, 0x234, 0x22C, 0x238, 0x23C, 0x230, 0x240, 0x244, 0x228, 0x354, 0x358, 0x35C, 0x050, 0x04C, 0x054, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x340, 0x33C, 0x348, 0x34C, 0x350, 0x344, 0x1D0, 0x1D4, 0x1D8, 0x1DC, 0x1E0, 0x1E4, 0x1E8, 0x1EC, 0x1F0, 0x1F4},
/*2*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x2FC, 0x300, 0x304, 0x308, 0x30C, 0x310, 0x314, 0x318, 0x31C, 0x320, 0x324, 0x328, 0x32C, 0x330, 0x334, 0x338, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x0DC, 0x0E0, 0x0E4, 0x0E8, 0x0EC, 0x0F0, 0x0F4, 0x0F8, 0x0FC, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x08C, 0x0B0},
/*3*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x114, 0x118, 0x11C, 0x120, 0x124, 0x128, 0x12C, 0x130, 0x134, 0x138, 0x13C, 0x140, 0x144, 0x148, 0x14C, 0x150,
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x090, 0x094, 0x098, 0x09C, 0x0A0, 0x0A4, 0x0A8, 0x0AC, 0x0B4, 0x0B8, 0x0BC, 0x0C0, 0x0C4, 0x0C8, 0x0CC, 0x0D0},
/*4*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x000, 0x000, 0x000, 0x000, 0x000, 0x254, 0x1F8, 0x1FC, 0x200, 0x204, 0x208, 0x20C, 0x210, 0x214, 0x218, 0x21C, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x15C, 0x160, 0x164, 0x168, 0x16C, 0x170, 0x174, 0x178, 0x17C, 0x180, 0x184, 0x188, 0x18C, 0x190, 0x194, 0x198},
/*5*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x154, 0x000, 0x088, 0x000, 0x0D4, 0x19C, 0x1A0, 0x1A4, 0x1A8, 0x1AC, 0x1B0, 0x1B4, 0x1B8, 0x1BC, 0x1C0, 0x1C4, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x1C8, 0x1CC, 0x258, 0x25C, 0x260, 0x264, 0x268, 0x26C, 0x270, 0x274, 0x278, 0x27C, 0x280, 0x284, 0x288, 0x28C},
/*6*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x290, 0x294, 0x298, 0x29C, 0x2A0, 0x2A4, 0x0D8, 0x2D4, 0x2D8, 0x2DC, 0x2E0, 0x2E4, 0x000, 0x000, 0x2E8, 0x2EC, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x2F0, 0x2A8, 0x2AC, 0x058, 0x05C, 0x060, 0x064, 0x068, 0x06C, 0x070, 0x074, 0x078, 0x07C, 0x080, 0x084, 0x158},
/*7*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */     
    {0x2B0, 0x2B4, 0x2B8, 0x2BC, 0x2C0, 0x2C4, 0x2C8, 0x2CC, 0x2D0, 0x2F4, 0x2F8, 0x248, 0x24C, 0x250, 0x000, 0x000, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000}
};

static const uint16_t IOMUX_PAD_RegTab[7][32] = 
{
/*1*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */        
    {0x5F0, 0x5F4, 0x604, 0x5fc, 0x608, 0x60c, 0x600, 0x610, 0x614, 0x5f8, 0x73c, 0x740, 0x744, 0x364, 0x360, 0x368,
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x728, 0x724, 0x730, 0x734, 0x738, 0x72c, 0x4e4, 0x4e8, 0x4ec, 0x4f0, 0x4f4, 0x4f8, 0x4fc, 0x500, 0x504, 0x508},
/*2*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x6e4, 0x6e8, 0x6ec, 0x6f0, 0x6f4, 0x6f8, 0x6fc, 0x700, 0x704, 0x708, 0x70c, 0x710, 0x714, 0x718, 0x71c, 0x720, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x3f0, 0x3f4, 0x3f8, 0x3fc, 0x400, 0x404, 0x408, 0x40c, 0x410, 0x414, 0x418, 0x41c, 0x420, 0x424, 0x3a0, 0x3c4},
/*3*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x428, 0x42c, 0x430, 0x434, 0x438, 0x43c, 0x440, 0x444, 0x448, 0x44c, 0x450, 0x454, 0x458, 0x45c, 0x460, 0x464,
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x3a4, 0x3a8, 0x3ac, 0x3b0, 0x3b4, 0x3b8, 0x3bc, 0x3c0, 0x3c8, 0x3cc, 0x3d0, 0x3d4, 0x3d4, 0x3dc, 0x3e0, 0x3e4},
/*4*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x000, 0x000, 0x000, 0x000, 0x000, 0x624, 0x5c8, 0x5cc, 0x5d0, 0x5d4, 0x5d8, 0x5dc, 0x5e0, 0x5e4, 0x5e8, 0x5ec, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x470, 0x474, 0x478, 0x47c, 0x480, 0x484, 0x488, 0x48c, 0x490, 0x494, 0x498, 0x49c, 0x4a0, 0x4a4, 0x4a8, 0x4ac},
/*5*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x468, 0x000, 0x39c, 0x000, 0x3e8, 0x4b0, 0x4b4, 0x4b8, 0x4bc, 0x4c0, 0x4c4, 0x4c8, 0x4cc, 0x4d0, 0x4d4, 0x4d8, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x4dc, 0x4e0, 0x628, 0x62c, 0x630, 0x634, 0x638, 0x63c, 0x640, 0x644, 0x648, 0x64c, 0x650, 0x654, 0x658, 0x65c},
/*6*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */      
    {0x660, 0x664, 0x668, 0x66c, 0x670, 0x674, 0x3ec, 0x6bc, 0x6c0, 0x6c4, 0x6c8, 0x6cc, 0x000, 0x000, 0x6d0, 0x6d4, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x6d8, 0x690, 0x694, 0x36c, 0x370, 0x374, 0x378, 0x37c, 0x380, 0x384, 0x388, 0x38c, 0x390, 0x394, 0x398, 0x46c},
/*7*//*0     1       2      3      4      5      6      7      8      9      10     11     12     13     14     15   */     
    {0x698, 0x69c, 0x6a0, 0x6a4, 0x6a8, 0x6ac, 0x6b0, 0x6b4, 0x6b8, 0x6d8, 0x6e0, 0x618, 0x61c, 0x620, 0x000, 0x000, 
     /*16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31   */
     0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000}     
};

static bool gpio_init(uintptr_t mux_vaddr, uint16_t port, uint16_t pin)
{
    if (port >= GPIO_BANK_NUM || pin >= 32)
        return false;
    if (!IOMUX_MUX_RegTab[port][pin])
        return false;

    out32(mux_vaddr + IOMUX_MUX_RegTab[port][pin], 0x05);
    out32(mux_vaddr + IOMUX_PAD_RegTab[port][pin], 0x0001B030);
    return true;
}

static bool get_reg_bit(uintptr_t vaddr, uint16_t bit)
{
    return in32(vaddr) & (1 << bit);
}

static void set_reg_bit(uintptr_t vaddr, uint16_t bit, bool high)
{
    if (high)
        out32(vaddr, in32(vaddr) | (1 << bit));
    else
        out32(vaddr, in32(vaddr) & ~(1 << bit));
}

int main(int argc, char* argv[])
{
    const uint16_t user_led_pin[] = {2, 3, 4};   // User LED is 2-2, 2-3, 2-4 in Digi board
    uint16_t port = USER_LED_PORT;
    error_t err = EOK;
    paddr_t gpio_base_paddr = gpio_base[port];
    uint16_t pin = user_led_pin[0];
    uint32_t times = USER_LED_FLICKS;
    int period = 200;

    xprintf("------------------------ proc%d: %s ------------------------\n", g_procEnv.pid, argv[0]);

    if (argc > 1)
    {
        if (!strcmp(argv[1], "?") || !strcmp(argv[1], "-h"))
        {
            xprintf("Usage: %s [led index] [times] [period]\n", argv[0]);
            return err;
        }
        uint16_t index = (uint16_t)atoi(argv[1]);

        if (index >= ARRAY_SIZE(user_led_pin))
            index = ARRAY_SIZE(user_led_pin) - 1;
        pin = user_led_pin[index];
        if (argc > 2)
        {
            times = (uint32_t)atoi(argv[2]);
            if (!times)
                times = 1;
            if (argc > 3)
            {
                period = atoi(argv[3]);
                if (period <= 0)
                    period = USER_LED_PERIOD;
            }
        }
    }

    uintptr_t mux_base = NULL_VADDR, gpio_base = NULL_VADDR;

    mux_base = (uintptr_t)mmap_device_io(IOMUXC_SIZE, IOMUXC_BASE);
    if (MAP_FAILED == mux_base)
    {
        xprintf("Map user LED gpio mux: %x failed\n", IOMUXC_BASE);
        err = ERR_NO_MEMORY;
        goto mapio_end;
    }

    if (!gpio_init(mux_base, port, pin))
    {
        xprintf("Invalid user LED gpio port:%d, pin:%d failed\n", (int)port, (int)pin);
        err = ERR_INVALID_ARG;
        goto mapio_end;
    }

    gpio_base = (uintptr_t)mmap_device_io(GPIO_REG_SIZE, gpio_base_paddr);
    if (MAP_FAILED == gpio_base)
    {
        xprintf("Map user LED gpio port: %x failed\n", gpio_base_paddr);
        err = ERR_NO_MEMORY;
        goto mapio_end;
    }

    // set LED pin as output
    set_reg_bit(gpio_base + GPIO_GDIR, pin, true);
    if (!get_reg_bit(gpio_base + GPIO_GDIR, pin))
    {
        xprintf("User LED %d-%d set output failed\n", (int)port, (int)pin);
        err = ERR_UNAVAILABLE;
        goto mapio_end;
    }

    xprintf("User LED flicks for %d times with period %dms\n", times, period);
    for (uint32_t i = 0; i < times; i++)
    {
        set_reg_bit(gpio_base + GPIO_DR, pin, true);
        msleep(period);
        set_reg_bit(gpio_base + GPIO_DR, pin, false);
        msleep(period);
    }
    xprintf("User LED flicks over\n");

mapio_end:
    if (NULL_VADDR != mux_base)
        munmap((void*)mux_base, IOMUXC_SIZE);
    if (NULL_VADDR != gpio_base)
        munmap((void*)gpio_base, GPIO_REG_SIZE);
    xprintf("------------------------ %s test over ------------------------\n", argv[0]);
    return err;
}
