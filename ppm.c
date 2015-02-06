#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h> 
#include <asm/io.h>

#include "rpi.h"

/* Later on, the assigned IRQ numbers for the buttons are stored here */
static int button_irqs[] = { -1, -1 };

/*
 * The interrupt service routine called on button presses
 */
static irqreturn_t button_isr(int irq, void *data)
{
	if(irq == button_irqs[0]) {
        int value = GPIO_READ(24) ? 1: 0;
		if (value == 1) {
            GPIO_SET_LOW = 1 << 24;
        } else {
            GPIO_SET_HIGH = 1 << 24;
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

    INP_GPIO(4);	

	INP_GPIO(24);
	OUT_GPIO(24);
        
	// register LED gpios
	ret = gpio_request(24, "PIN 24" );

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs for LEDs: %d\n", ret);
		return ret;
	}
	
	// register BUTTON gpios
	ret = gpio_request(4, "PPM PIN");

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs for PPM: %d\n", ret);
		goto fail1;
	}

	printk(KERN_INFO "Current PPM value: %d\n", GPIO_READ(4) ? 1: 0);
	
	ret = gpio_to_irq(4);

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
	gpio_free(24);

fail1:
	gpio_free(4);

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
	

    GPIO_SET_LOW = 1 << 24;
	//for(i = 0; i < ARRAY_SIZE(leds); i++) {
	//	gpio_set_value(leds[i].gpio, 0); 
	//}
	
	gpio_free(24);
	gpio_free(4);

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
