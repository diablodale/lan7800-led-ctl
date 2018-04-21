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

#include <stdio.h>
#include <stdlib.h>		/* exit() */
#include <string.h>		/* strlen() */
#include <stdint.h>
#include <libusb-1.0/libusb.h>

#include "lan7800-led-ctl.h"

/* global variables */
int led_arr[3] = { MODE_KEEP, MODE_KEEP, MODE_KEEP };

libusb_device_handle *usbdev_open(int vid, int pid)
{
	libusb_device **devs;
	libusb_device *dev;
	libusb_device_handle *handle = NULL;
	struct libusb_device_descriptor desc;
	int r,i=0;
	
	if ((libusb_init(NULL)) < 0) {
		fprintf(stderr, "libusb_init() failed\n");
		return NULL;
	}
	if (libusb_get_device_list(NULL, &devs) < 0) {
		return NULL;
	}
	while ((dev=devs[i++]) != NULL) {
		if ((r=libusb_get_device_descriptor(dev, &desc)) < 0) {
			fprintf(stderr, "failed to get device descriptor");
			libusb_free_device_list(devs, 1);
			return NULL;
		}
		if ((desc.idVendor == vid) && (desc.idProduct == pid)) {
			if ((r=libusb_open(dev, &handle)) != 0) {
				fprintf(stderr, "libusb_open error :%s\n", libusb_error_name(r));
				return NULL;
			}
			libusb_free_device_list(devs, 1);
			return handle;
		}
	}
	libusb_free_device_list(devs, 1);
	return NULL;
}

int lan7800_rd_reg(libusb_device_handle *h, uint16_t reg, uint32_t *val)
{
	return libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_VENDOR|0x80,
		USB_VENDOR_REQUEST_RD_REG, 0, reg, (uint8_t *)val, 4,
		USB_CTRL_TIMEOUT);
}

int lan7800_wr_reg(libusb_device_handle *h, uint16_t reg, uint32_t val)
{
	return libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_VENDOR,
		USB_VENDOR_REQUEST_WR_REG, 0, reg, (uint8_t *)&val, 4,
		USB_CTRL_TIMEOUT);
}

int ledmode(const char* s)
{
	if (*s == '0') {
		return MODE_OFF;
	} else if (*s == '1') {
		return MODE_ON;
	} else if ((*s == 's') || (*s == 'S')) {
		return MODE_STATUS;
	}
	return MODE_ERR;
}

void usage(void)
{
	printf("usage: lan7800-led-ctl [--led0=x][--led1=x]\n");
	printf("\twhere x is one of:\n");
	printf("\t0 - turn LED off\n\t1 - turn LED on\n\ts - LED shows status\n");
	exit(1);
}

void about(void)
{
	printf("lan7800-led-ctl %s programmed by Dominic Radermacher\n", VERSION);
	printf("For further info or latest version see\n");
	printf("https://mockmoon-cybernetics.ch/computer/raspberry-pi/lan7800-led-ctl/\n");
	exit(1);
}

int parse_args(int argc, char **argv)
{
	int i;

	for (i=1; i<argc; i++) {
		char *p=argv[i];
		if (strncmp(p, "--led0=", 7) == 0) {
			led_arr[0] = ledmode(p+7);
			printf("setting LED0 to status %i\n", led_arr[0]);
		} else if (strncmp(p, "--led1=", 7) == 0) {
			led_arr[1] = ledmode(p+7);
			printf("setting LED1 to status %i\n", led_arr[1]);
		} else if (strcmp(p, "--about") == 0) {
			about();
		} else if (strcmp(p, "--version") == 0) {
			about();
		} else {
			usage();
		}
	}
	return i;
}

int main(int argc, char *argv[])
{
	libusb_device_handle *handle = NULL;
	uint32_t reg_hw_cfg;
	uint32_t gp_mask[2];

	gp_mask[0] = LED0_MASK;
	gp_mask[1] = LED1_MASK;
	if (argc < 2) {
		printf("at least one argument is required\n");
		usage();
	}
	parse_args(argc, argv);
	if ((handle=usbdev_open(LAN7800_VENDOR_ID, LAN7800_PRODUCT_ID)) == NULL) {
		printf("No LAN7800 found on USB (are you root?)\n");
		exit(1);
	}
	lan7800_rd_reg(handle, REG_HW_CFG, &reg_hw_cfg);
#ifdef DEBUG
	printf("read HW_CFG register: %08x\n", reg_hw_cfg);
#endif
	for (int i=0; i < 2; i++) {
		if (led_arr[i] == MODE_ON) {
			reg_hw_cfg &= ~(gp_mask[i] & HW_CFG_LED_EN);
			printf("turning LED on not supported yet\n");
		}
		if (led_arr[i] == MODE_OFF) {
			reg_hw_cfg &= ~(gp_mask[i] & HW_CFG_LED_EN);
 		}
		if (led_arr[i] == MODE_STATUS) {
			reg_hw_cfg |= (gp_mask[i] & HW_CFG_LED_EN);
		}
	}
#ifdef DEBUG
	printf("write %08x to HG_CFG\n", reg_hw_cfg);
#endif
	lan7800_wr_reg(handle, REG_HW_CFG, reg_hw_cfg);
	libusb_close(handle);
}
