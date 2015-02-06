#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h> 
#include <asm/io.h>

#include "rpi.h"

#define PPM_PIN 4
#define TEST_PIN 24

/* Later on, the assigned IRQ numbers for the buttons are stored here */
static int button_irqs[] = { -1, -1 };


static inline void gpio_set(uint32_t pin) 
{
    GPIO_SET_HIGH = 1 << pin;
}

static inline void gpio_clear(uint32_t pin) 
{
    GPIO_SET_LOW = 1 << pin;
}

static inline bool gpio_read(uint32_t pin) 
{
    return GPIO_READ(pin) ? true: false;
}

static inline void gpio_set_input(uint32_t pin)
{
    INP_GPIO(pin);
}

static inline void gpio_set_output(uint32_t pin)
{
    OUT_GPIO(pin);
}

/*
 * The interrupt service routine called on button presses
 */
static irqreturn_t button_isr(int irq, void *data)
{
	if(irq == button_irqs[0]) {
        int value = gpio_read(24);
		if (value == 1) {
            gpio_clear(24);
        } else {
            gpio_set(24);
        }

	}

	return IRQ_HANDLED;
}

/*
 * Module init function
 */
static int __init ppm_init(void)
{
	int ret = 0;

	printk(KERN_INFO "%s\n", __func__);

    gpio.map     = ioremap(GPIO_BASE, 4096);//p->map;
	gpio.addr    = (volatile unsigned int *)gpio.map;

    printk(KERN_INFO "%p %p\n", gpio.map, gpio.addr);

    gpio_set_input(PPM_PIN);	

	gpio_set_input(TEST_PIN);
	gpio_set_output(TEST_PIN);
        
	// register LED gpios
	ret = gpio_request(TEST_PIN, "PIN TEST" );

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs for LEDs: %d\n", ret);
		return ret;
	}
	
	// register BUTTON gpios
	ret = gpio_request(PPM_PIN, "PPM PIN");

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs for PPM: %d\n", ret);
		goto fail1;
	}

	printk(KERN_INFO "Current PPM value: %d\n", GPIO_READ(PPM_PIN) ? 1: 0);
	
	ret = gpio_to_irq(PPM_PIN);

	if(ret < 0) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}

	button_irqs[0] = ret;

	printk(KERN_INFO "Successfully requested PPM PIN4 IRQ # %d\n", button_irqs[0]);

	ret = request_irq(button_irqs[0], button_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "ppm#pin4", NULL);

	if(ret) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}


	return 0;

// cleanup what has been setup so far

fail2: 
	gpio_free(PPM_PIN);

fail1:
	gpio_free(TEST_PIN);

	return ret;	
}

/**
 * Module exit function
 */
static void __exit ppm_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n", __func__);

	free_irq(button_irqs[0], NULL);
	
    /* Clear pin */
    gpio_clear(TEST_PIN);

	gpio_free(TEST_PIN);
	gpio_free(PPM_PIN);

    if (gpio.addr){
        /* release the mapping */
        iounmap(gpio.addr);
	}
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Georgii Staroselskii");
MODULE_DESCRIPTION("Linux Kernel module for PPM handling.");

module_init(ppm_init);
module_exit(ppm_exit);
