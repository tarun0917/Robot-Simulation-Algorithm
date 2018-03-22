/*
 * camera.c
 *
 * Created: 2/14/2018 4:26:29 PM
 *  Author: Tarun,Yang
 */ 




#include "camera.h"
#include <string.h>
#include "ov7740.h"

// All variables go here
volatile uint32_t vsync_rising_edge_flag = false ;
volatile uint8_t image_dest_buffer_ptr[CAM_BUFFER] = {0};
volatile uint32_t start_buff = 0;
volatile uint32_t image_length = 0;


// All functions go here
static void init_vsync_interrupts(void)
{
	//Initialize PIO interrupt handler, see PIO definition in conf_board.h
	pio_handler_set(OV_VSYNC_PIO, OV_VSYNC_ID, OV_VSYNC_MASK,OV_VSYNC_TYPE, vsync_handler);

	// Enable PIO controller IRQs
	NVIC_EnableIRQ((IRQn_Type)OV_VSYNC_ID);
}


static void vsync_handler(uint32_t ul_id, uint32_t ul_mask)
{
	unused(ul_id);
	unused(ul_mask);
	vsync_rising_edge_flag = true;
}

void pio_capture_init(Pio *p_pio, uint32_t ul_id)
{
	//Enable periphral clock 
	pmc_enable_periph_clk(ul_id);

	// Disable pio capture 
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN);

	// Disable rxbuff interrupt 
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF;

	// 32idth
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;

	// Only HSYNC and VSYNC enabled 
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);

}

static uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf,uint32_t ul_size)
{
	//Check if the first PDC bank is free 
	if ((p_pio->PIO_RCR == 0) && (p_pio->PIO_RNCR == 0)) {
		p_pio->PIO_RPR = (uint32_t)uc_buf;
		p_pio->PIO_RCR = ul_size;
		p_pio->PIO_PTCR = PIO_PTCR_RXTEN;
		return 1;
		} else if (p_pio->PIO_RNCR == 0) {
		p_pio->PIO_RNPR = (uint32_t)uc_buf;
		p_pio->PIO_RNCR = ul_size;
		return 1;
		} else {
		return 0;
	}
}

void configure_twi(void)
{   
	twi_options_t opt;
	// Enable TWI peripheral 
	pmc_enable_periph_clk(ID_BOARD_TWI);

	// Init TWI peripheral 
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = TWI_CLK;
	twi_master_init(BOARD_TWI, &opt);

	// Configure TWI interrupts 
	NVIC_DisableIRQ(BOARD_TWI_IRQn);
	NVIC_ClearPendingIRQ(BOARD_TWI_IRQn);
	NVIC_SetPriority(BOARD_TWI_IRQn, 0);
	NVIC_EnableIRQ(BOARD_TWI_IRQn);
}

void init_camera(void)
{
    pmc_enable_pllbck(7,0x1,1);
	// Init Vsync handler
	init_vsync_interrupts();

	// Init PIO capture
	pio_capture_init(OV_DATA_BUS_PIO, OV_DATA_BUS_ID);

	

	// Init PCK0 to work at 24 Mhz  96/4=24 Mhz 
	PMC->PMC_PCK[1] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLB_CLK);
	PMC->PMC_SCER = PMC_SCER_PCK1;
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK1)) {}
    configure_twi();
}

void configure_camera(void)
{
	 init_camera();
	// ov2640 Initialization 
	while (ov_init(BOARD_TWI) == 1) {
	}
	
	// ov2640 configuration 
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG_640x480);
}

 uint8_t start_capture(void)
{
	// Enable vsync interrupt*
	pio_enable_interrupt(OV_VSYNC_PIO, OV_VSYNC_MASK);

	/* Capture acquisition will start on rising edge of Vsync signal.
	 * So wait g_vsync_flag = 1 before start process
	 */
	while (!vsync_rising_edge_flag) {}

	// Disable vsync interrupt
	pio_disable_interrupt(OV_VSYNC_PIO, OV_VSYNC_MASK);

	// Enable pio capture
	pio_capture_enable(OV_DATA_BUS_PIO);

	pio_capture_to_buffer(OV_DATA_BUS_PIO,image_dest_buffer_ptr,CAM_BUFFER/4);
	
	// Wait end of capture
	while (!((OV_DATA_BUS_PIO->PIO_PCISR & PIO_PCIMR_RXBUFF) ==  PIO_PCIMR_RXBUFF)) {}

	// Disable pio capture
	pio_capture_disable(OV_DATA_BUS_PIO);

	// Reset vsync flag
	vsync_rising_edge_flag = false;
	
	image_length = find_image_len();
}

uint32_t find_image_len(void)   
{
	//The first loop is to find the starting address of the JPEG (0xFFD8)
	for(int i=0;i<CAM_BUFFER; i++){
		if((image_dest_buffer_ptr[i]==0xFF) && (image_dest_buffer_ptr[i+1]==0xD8)){
			start_buff = i;
			break;
		}
		else
		image_length = 0;
	}
	
	// Once the start of the JPEG image has been found, the end of the image is found (0xFFD9)
	int j=0;
	for (int i=start_buff;i<CAM_BUFFER;i++){
		if((image_dest_buffer_ptr[i]==0xFF) && (image_dest_buffer_ptr[i+1]==0xD9)){
			image_length = j+1 ;
			break;
		}
		j++;
	}
	if(image_length==0)
		return 0;
	else
		return image_length;
}