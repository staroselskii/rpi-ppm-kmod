#define BCM2708_PERI_BASE       0x20000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)	// GPIO controller 
#define TIMER_BASE              (0x20003000)
#define TIMER_OFFSET            (0x4)
 
 
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))
 
#define GPIO_SET_HIGH       *(gpio.addr + 7)  // sets   bits which are 1
#define GPIO_SET_LOW        *(gpio.addr + 10) // clears bits which are 1

#define GPIO_CLR  *(gpio.addr + 10) 
 
#define GPIO_READ(g)  *(gpio.addr + 13) & (1<<(g))

#define TIMER_GET   *(long long int *)((char *)timer.addr + TIMER_OFFSET)

struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

struct bcm2835_timer 
{
    unsigned long addr_p;
    void *map;
    volatile unsigned int *addr;
};

struct bcm2835_timer timer = {TIMER_BASE};
struct bcm2835_peripheral gpio = {GPIO_BASE};
