/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "camera.h"

void board_init(void)
{
	
	//Button pin Initialization
	ioport_set_pin_dir(LED_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_PIN, false);
	ioport_set_pin_dir(WIFI_BUTTON_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(WIFI_BUTTON_PIN,IOPORT_MODE_PULLUP);
    ioport_set_pin_level(WIFI_BUTTON_PIN, false);


	// Camera data pin Initialization
	gpio_configure_pin(OV_HSYNC_GPIO, OV_HSYNC_FLAGS);
	gpio_configure_pin(OV_VSYNC_GPIO, OV_VSYNC_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D2, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D3, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D4, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D5, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D6, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D7, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D8, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D9, OV_DATA_BUS_FLAGS);

	// Camera Initialization Pin 
	gpio_configure_pin(PIN_PCK0, PIN_PCK0_FLAGS);
	gpio_configure_pin(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
	gpio_configure_pin(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);
	gpio_configure_pin(CAM_RST, CAM_RST_FLAGS);	
	ioport_set_pin_mode(CAM_RST,IOPORT_MODE_PULLUP);
	ioport_set_pin_level(CAM_RST, false);
	
	
	gpio_configure_pin(NET_PIN, NET_PIN_FLAGS);
	ioport_set_pin_dir(NET_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(NET_PIN, IOPORT_MODE_PULLUP);
}
