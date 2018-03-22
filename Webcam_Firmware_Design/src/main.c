#include "wifi.h"
#include "camera.h"
#include "timer_interface.h"
#include <conf_clock.h>
#include <conf_board.h>


int main (void)
{	
	// All the initialization goes here, the individual pins are intialised in the init.c function
	sysclk_init();
	wdt_disable(WDT);
	board_init();
	ioport_init();
	configure_tc();
	tc_start(TC0,0);
	
	//All configuration goes here
	configure_usart_wifi();
	configure_wifi_comm_pin();
	configure_wifi_web_setup_pin();
	configure_camera();
	
	
	// This is to setup the wifi pins and the LED indicators and root folder of the Wifi server
	usart_write_line(BOARD_USART,"set uart.flow 0 on\r\n");
    usart_write_line(BOARD_USART,"set bu c r 5000\r\n");
    usart_write_line(BOARD_USART,"set system.indicator.gpio wlan 20\r\n");
    usart_write_line(BOARD_USART,"set system.indicator.gpio network 18\r\n");
    usart_write_line(BOARD_USART,"set system.indicator.gpio softap 21\r\n");
    usart_write_line(BOARD_USART,"set set wl n o 16\r\n");
    usart_write_line(BOARD_USART,"set sy c e off\r\n");
    usart_write_line(BOARD_USART,"set hhtp.sever.root_filename webcam/main.html\r\n");
    usart_write_line(BOARD_USART,"save\r\n");
    usart_write_line(BOARD_USART,"reboot\r\n");
	
	
	
	//This is the reboot function, a delay of 10s is added so that it immediately doesn't go into the while loop and gives sufficient time for the reboot of the wifi chip to occur
	wifi_button_flag = false;
	wifi_status_flag = false;
	net_flag = false;
 	usart_write_line(BOARD_USART,"reboot\r\n");
 	delay_ms(10000);
 	
	//While loop
	while(1)
	{
	
	  //This is the setup web function which is used to setup and connect the wifi chip to a desired network connection 
	  if(wifi_button_flag)
		{
 		usart_write_line(BOARD_USART,"setup web\r\n");
		delay_ms(1000);
		wifi_button_flag=false;
		wifi_status_flag = ioport_get_pin_level(NET_PIN);
		while(!wifi_status_flag)
		{
	   //Keep checking for the pin status till it is not connected the network
		wifi_status_flag = ioport_get_pin_level(NET_PIN);
		}
	    delay_ms(10000);
		}
	
		usart_write_line(BOARD_USART,"poll all\r\n");
		delay_ms(1000);
	  //Capture of an image		
		while(net_flag)
		{
		delay_ms(1);	
		start_capture();
		if(image_length>0)
		{
 		write_image_to_file();
		}
	  if(wifi_button_flag)
		{
 		usart_write_line(BOARD_USART,"setup web\r\n");
		delay_ms(1000);
		wifi_button_flag=false;
		wifi_status_flag = ioport_get_pin_level(NET_PIN);
		while(!wifi_status_flag)
		{
		wifi_status_flag = ioport_get_pin_level(NET_PIN);
		}
	    delay_ms(10000);
		}
		}
		
	}
}
