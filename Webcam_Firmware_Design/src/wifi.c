/*
 * wifi.c
 *
 * Created: 2/9/2018 1:24:00 PM
 *  Author: Tarun,Yang
 */ 

#include "wifi.h"
#include <string.h>
#include "camera.h"


// All variables go here
volatile uint32_t received_byte_wifi =0;
volatile uint32_t input_pos_wifi = 0; 
volatile bool wifi_button_flag = false ;
volatile bool wifi_status_flag = false;
volatile bool new_rx_wifi =false;
volatile bool wifi_flag = false;
volatile bool net_flag = false;
volatile bool transfer_start = false;

volatile uint8_t timeout_counter = 0;
volatile uint8_t  data_recieved = 0;


// All functions go here
void USART_Handler(void)
{
	uint32_t ul_status;


	// Read USART status. 
	ul_status = usart_get_status(BOARD_USART);

	// Receive buffer is full. 
	if (ul_status & US_CSR_RXBUFF) {
		usart_read(BOARD_USART,&received_byte_wifi);
		new_rx_wifi = true;
		process_incoming_byte_wifi((uint8_t)received_byte_wifi);
		}
}

static void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask)
{
	unused(ul_id);
	unused(ul_mask);
	
	process_data_wifi();
	for (uint32_t i=0;i<MAX_INPUT_WIFI;i++) 
	{input_line_wifi[i] =0;}
	input_pos_wifi = 0;
	
}

static void wifi_web_setup_handler(uint32_t ul_id1, uint32_t ul_mask1)
{
	unused(ul_id1);
	unused(ul_mask1);
	wifi_button_flag = true;
}

void configure_usart_wifi(void)
{    
	gpio_configure_pin(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
	gpio_configure_pin(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);
	gpio_configure_pin(PIN_USART0_CTS_IDX, PIN_USART0_CTS_FLAGS);
	gpio_configure_pin(PIN_USART0_RTS_IDX, PIN_USART0_RTS_FLAGS);

	static uint32_t ul_sysclk;
	const sam_usart_opt_t usart_console_settings = {
		BOARD_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		// This field is only used in IrDA mode. 
	};

	// Get peripheral clock. 
	ul_sysclk = sysclk_get_peripheral_hz();

	// Enable peripheral clock. 
	sysclk_enable_peripheral_clock(BOARD_ID_USART);

	// Configure USART. 
	usart_init_hw_handshaking(BOARD_USART, &usart_console_settings, ul_sysclk);

	// Disable all the interrupts. 
	usart_disable_interrupt(BOARD_USART, ALL_INTERRUPT_MASK);
	
	// Enable TX & RX function. 
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);    
	// Configure and enable interrupt of USART. 
	NVIC_EnableIRQ(USART_IRQn);
}

void configure_wifi_comm_pin(void)
{
	// Configure PIO clock. 
	pmc_enable_periph_clk(WIFI_COMM_ID);

	// Adjust PIO debounce filter using a 10 Hz filter. 
	pio_set_debounce_filter(WIFI_COMM_PIO, WIFI_COMM_MASK, 10);

	// Initialize PIO interrupt handler, see PIO definition in conf_board.h

	pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_MASK,
			WIFI_COMM_ATTR, wifi_command_response_handler);

	// Enable PIO controller IRQs. 
	NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);

	// Enable PIO interrupt lines. 
	pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_MASK);
	data_recieved = 0;
}

void configure_wifi_web_setup_pin(void)
{
	// Configure PIO clock. 
	pmc_enable_periph_clk(WIFI_WEB_ID);

	// Adjust PIO debounce filter using a 10 Hz filter. 
	pio_set_debounce_filter(WIFI_WEB_PIO, WIFI_WEB_PIN_MSK, 10);

	// Initialize PIO interrupt handler, see PIO definition in conf_board.h
	pio_handler_set(WIFI_WEB_PIO, WIFI_WEB_ID, WIFI_WEB_PIN_MSK,
			WIFI_WEB_ATTR, wifi_web_setup_handler);

	// Enable PIO controller IRQs. 
	NVIC_EnableIRQ((IRQn_Type)WIFI_WEB_ID);

	// Enable PIO interrupt lines. 
	pio_enable_interrupt(WIFI_WEB_PIO, WIFI_WEB_PIN_MSK);
}

void process_incoming_byte_wifi(uint8_t in_byte)
{
	input_line_wifi[input_pos_wifi++]=in_byte;
	
}

void process_data_wifi()
{
	
	if(strstr(input_line_wifi,"0,0"))
	{
		net_flag = true;
	}
	
	if(strstr(input_line_wifi,"None"))
	{
		net_flag = false;
	}
	
	if(strstr(input_line_wifi,"Websocket disconnected"))
	{
		net_flag = false;
	}
	
	//To detect if the image_transfer function was given whose output are the words Start transfer
	if(strstr(input_line_wifi,"Start transfer"))
	{
		transfer_start = true;
	}
	
	if(strstr(input_line_wifi,"Complete"))
	{
		//This is the 50ms delay which is given after the image_transfer is complete
		delay_ms(50);
	}
	
}

void write_wifi_command(char*comm, uint8_t cnt)
{
	data_recieved = 0 ;
	usart_write_line(BOARD_USART, comm);
	
	timeout_counter = 0;
	cnt = cnt * 20 ;
	while(timeout_counter<cnt && !data_recieved){
	delay_ms(10);
	timeout_counter++;
	}
	  
	  
	
}

void write_image_to_file()
{
	// if there is an image detected where the length exists
	if(image_length!= 0)														
	{
		// create a string that contains 20 chars can hold the command
		char image_length_transfer[20];
		
		// put "image transfer" and image length together to make the final command
		sprintf(image_length_transfer,"image_transfer %d\r\n",image_length);	
		
		// write the command to the WIFI moduel
		write_wifi_command(image_length_transfer,2);							
		while(transfer_start)													
		{
			// create an array for transfer content with a length that detected for that specific image
			uint8_t image_transfer[image_length];								
			char tem_char;															
			uint8_t tem_int;													
			for(uint32_t i = 0 ;i<image_length;i++)								
			{
				// put the content on the address starting at the starting point to the tem variable
				tem_int = image_dest_buffer_ptr[start_buff+i];					
				tem_char = tem_int;			
				// output the char tem to the terminal									
				usart_putchar(BOARD_USART,tem_char);								
			}
			transfer_start = false;												
		}
	}
}













