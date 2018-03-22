/*
 * wifi.h
 *
 * Created: 2/9/2018 1:24:10 PM
 *  Author: Tarun,Yang
 */ 


#ifndef WIFI_H_
#define WIFI_H_
#include <asf.h>
#include <string.h>
#include "camera.h"
#include "wifi.h"

#define ALL_INTERRUPT_MASK  0xffffffff
#define MAX_INPUT_WIFI 1000

// All variables go here
volatile uint32_t received_byte_wifi;
volatile uint32_t input_pos_wifi;
volatile char input_line_wifi[MAX_INPUT_WIFI];
volatile bool new_rx_wifi ;
volatile bool wifi_button_flag;
volatile bool wifi_status_flag;
volatile bool wifi_flag;
volatile bool net_flag;
volatile bool transfer_start ;


// All functions go here
static void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask);
static void wifi_web_setup_handler(uint32_t ul_id1, uint32_t ul_mask1);
void configure_usart_wifi(void);
void configure_wifi_comm_pin(void);
void configure_wifi_web_setup_pin(void);
void process_incoming_byte_wifi(uint8_t in_byte);
void process_data_wifi(void);
void write_wifi_command(char*comm, uint8_t cnt);
void write_image_to_file(void);


#define BOARD_ID_USART             ID_USART0
#define BOARD_USART                USART0
#define BOARD_USART_BAUDRATE       115200
#define USART_Handler              USART0_Handler
#define USART_IRQn                 USART0_IRQn

/** USART0 pin RX */
#define PIN_USART0_RXD    {PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RXD_IDX        (PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/** USART0 pin TX */
#define PIN_USART0_TXD    {PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_TXD_IDX        (PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/** USART0 pin CTS */
#define PIN_USART0_CTS    {PIO_PA8A_CTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_CTS_IDX        (PIO_PA8_IDX)
#define PIN_USART0_CTS_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/** USART0 pin RTS */
#define PIN_USART0_RTS    {PIO_PA7A_RTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RTS_IDX        (PIO_PA7_IDX)
#define PIN_USART0_RTS_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)

#define WIFI_COMM_MASK          PIO_PB2
#define WIFI_COMM_PIO           PIOB
#define WIFI_COMM_ID            ID_PIOB
#define WIFI_COMM_TYPE          PIO_INPUT
#define WIFI_COMM_ATTR          PIO_PULLUP | PIO_DEBOUNCE |PIO_IT_RISE_EDGE

#define WIFI_WEB_ID                 ID_PIOA
#define WIFI_WEB_PIO                PIOA
#define WIFI_WEB_PIN_MSK            PIO_PA12
#define WIFI_WEB_ATTR               PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE


#define  NET_PIN (PIO_PB0_IDX)
#define NET_PIN_FLAGS (PIO_INPUT | PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

#endif /* WIFI_H_ */