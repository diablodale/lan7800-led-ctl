/*
	lan7800-led-ctl - control LEDs of LAN7800 ethernet/usb controllers
	
	Copyright (C) 2018 Dominic Radermacher <dominic@familie-radermacher.ch>
	
	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 3 as
	published by the Free Software Foundation
	
	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define USB_CTRL_TIMEOUT	(5000)
#define LAN7800_VENDOR_ID	(0x0424)
#define	LAN7800_PRODUCT_ID	(0x7800)

/* values from LAN7800 databook */
#define USB_VENDOR_REQUEST_WR_REG	(0xa0)
#define USB_VENDOR_REQUEST_RD_REG	(0xa1)

/* register adresses - from LAN7800 databook */
#define REG_HW_CFG			(0x10)	/* hardware config register */
/* bit 20-23: LED0-LED3 enable */
#define REG_GPIO_CFG0			(0x18)
/* bit 16-31 reserved
   bit 12-15: GPIO0-3 enable; 1=LED, 0=GPIO
	bit 12 = GPIOEN0, bit 13= GPIOEN1, bit 14=GPIOEN2
   bit 8-11: GPIO0-3 buffer type, 1=push-pull, 0=open-drain
   bit 4-7: GPIO0-3 direction, 1=output, 0=input
   bit 0-3: GPIO0-3 data
   */

/* meaning of the (relevant) bits in the HW_CFG register */
#define HW_CFG_LED0_EN	(1<<20)
#define HW_CFG_LED1_EN	(1<<21)
#define HW_CFG_LED2_EN	(1<<22)
#define HW_CFG_LED3_EN	(1<<23)
/* meaning of the (relevant) bits in the GPIO_CFG0 register */
#define GPIO_CFG0_GPIOEN0	(1<<12)
#define GPIO_CFG0_GPIOEN1	(1<<13)
#define GPIO_CFG0_GPIOBUF0	(1<<8)
#define GPIO_CFG0_GPIOBUF1	(1<<9)
#define GPIO_CFG0_GPIODIR0	(1<<4)
#define GPIO_CFG0_GPIODIR1	(1<<5)
#define GPIO_CFG0_GPIOD0	(1<<0)
#define GPIO_CFG0_GPIOD1	(1<<1)

/* */
#define HW_CFG_LED_EN		(HW_CFG_LED0_EN|HW_CFG_LED1_EN)

#define LED0_MASK	(HW_CFG_LED0_EN|GPIO_CFG0_GPIOD0|GPIO_CFG0_GPIODIR0|GPIO_CFG0_GPIOBUF0|GPIO_CFG0_GPIOEN0)
#define LED1_MASK	(HW_CFG_LED1_EN|GPIO_CFG0_GPIOD1|GPIO_CFG0_GPIODIR1|GPIO_CFG0_GPIOBUF1|GPIO_CFG0_GPIOEN1)

#define MODE_OFF	0
#define MODE_ON		1
#define MODE_STATUS	2
#define MODE_KEEP	4
#define MODE_ERR	-1

void usage(void);
void about(void);
int ledmode(const char* str);
int parse_args(int argc, char **argv);
libusb_device_handle *usbdev_open(int vid, int pid);
int lan7800_rd_reg(libusb_device_handle *h, uint16_t reg, uint32_t *val);
int lan7800_wr_reg(libusb_device_handle *h, uint16_t reg, uint32_t val);
