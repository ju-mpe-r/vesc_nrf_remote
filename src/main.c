#include "main.h"
#include "refactoring.h"
//----------refactored--------------
#include "ss495a.h"
#include "pairing.h"
#include "mote_display.h"
#include "debug_feedback.h"

//----------end-of-refactored--------------


#include "stm32f1xx_hal.h" //подключает весь пакет HAL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include "gpio.h"
#include "rf.h"
#include "i2c.h"
#include "dma.h"
#include "tim.h"

#include "ssd1306.h"
#include "fonts.h"
#include "icons.h"

#include "stm32f1xx_it.h"
#include "init.h"


#include "datatypes.h"
#include "bldc_interface.h"
#include "conf_general.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "spi_sw.h"
#include "rf.h"
#include "rfhelp.h"
#include "buffer.h"
#include "packet.h"
#include "crc.h"
#include <stdarg.h>


#define MAX_PL_LEN				25
#define RX_BUFFER_SIZE			PACKET_MAX_PL_LEN

// Don't use ack on tx (requires the same setting on the vesc too)
#define NOACK					0

static uint8_t rx_buffer[RX_BUFFER_SIZE];



// Functions
static void send_buffer_nrf(unsigned char *data, unsigned int len);









static void send_packet(unsigned char *data, unsigned int len) {
	// Wait for the previous transmission to finish.
	// while (UARTD1.txstate == UART_TX_ACTIVE) {
	// 	chThdSleep(1);
	// }

	// uartStartSend(&UARTD1, len, data);
}









//DISPLAY-DOX-PART-START
//https://fontforge.github.io/en-US/documentation/


//BITMAP-FONTS
//https://thenextweb.com/dd/2012/01/31/get-your-bitmap-on-with-these-11-surprisingly-good-pixel-typefaces/
//https://github.com/Tecate/bitmap-fonts
//http://www.webdesigndev.com/pixel-fonts-bitmap-fonts/



//NRF-NA-RUSSKOM
//http://blog.sci-smart.ru/2013/06/nrf24l01.html
//https://github.com/wendlers/libemb/tree/master/libnrf24l01
//https://github.com/nt4t/Chibios-STM32F103-nRF24l01-spi-server-sb
//http://www.cyberforum.ru/arm/thread2091410.html
//https://github.com/petoknm/NRF24L01
//https://github.com/LonelyWolf/stm32/tree/master/nrf24l01

//!!!!!!!!!!!!!!!!!!!!!!!!!!!
//https://github.com/vedderb/nrf24l01plus_stm32f100_chibios
//https://github.com/vedderb/nrf24l01plus_stm32f100_chibios
//https://github.com/vedderb/nrf24l01plus_stm32f100_chibios





//https://github.com/FortAwesome/Font-Awesome/tree/master/advanced-options/raw-svg/brands


//https://habrahabr.ru/post/313490/
//https://github.com/SL-RU/sdmplayer/tree/master/src/v002/Hardware

//https://bitbucket.org/nadyrshin_ryu/ssd1306_stm32/src

//https://github.com/galpavlin/STM32-SSD1306/tree/master/128x64


//http://forum.easyelectronics.ru/viewtopic.php?f=35&t=14117

//DISPLAY-DOX-PART-END



// JLinkRTTLogger -device STM32F103C8 -RTTChannel 0 -if swd -speed 4000 /dev/stdout
// dont forget copy linker script from cubemx to
//cp .platfromio/packages/framework-stm32cube/platformio/ldscripts/STM32F103CI_FLASH.ld



//http://www.electric-skateboard.builders/t/vesc-nunchuk-rf/588/30
//https://github.com/vedderb/nunchuk_mod
//http://www.electric-skateboard.builders/t/vesc-nunchuk-rf/588/147
//http://vedder.se/forums/viewtopic.php?f=6&t=248&sid=4975f8e1079a2108e1944f30dfafb2bb&start=10


UART_HandleTypeDef huart1;
/* Private function prototypes -----------------------------------------------*/
static void MX_USART1_UART_Init(void);
/* Private function prototypes -----------------------------------------------*/


volatile uint32_t SS495_ADC_buffer = 0;;  //сделать 16битной







int main(void) {
	stm32_peripherals_init();
	remote_peripherals_init();



// startup_logo();


void send_buffer_nrf(unsigned char *data, unsigned int len) {
	uint8_t send_buffer[MAX_PL_LEN];


	if (data[0] == COMM_GET_VALUES) {
		nrf_stats.req_values++;
	}

	if (len <= (MAX_PL_LEN - 1)) {
		// SEGGER_RTT_printf(0, "SEND send_buffer_nrf MOTE_PACKET_PROCESS_SHORT_BUFFER\n");
		uint32_t ind = 0;
		send_buffer[ind++] = MOTE_PACKET_PROCESS_SHORT_BUFFER;
		memcpy(send_buffer + ind, data, len);
		ind += len;
		rf_tx_wrapper((char*)send_buffer, ind);
	} else {
		unsigned int end_a = 0;
		unsigned int len2 = len - (MAX_PL_LEN - 5);

		for (unsigned int i = 0;i < len2;i += (MAX_PL_LEN - 2)) {
			if (i > 255) {
				break;
			}

			end_a = i + (MAX_PL_LEN - 2);

			uint8_t send_len = (MAX_PL_LEN - 2);
			send_buffer[0] = MOTE_PACKET_FILL_RX_BUFFER;
			send_buffer[1] = i;

			if ((i + (MAX_PL_LEN - 2)) <= len2) {
				memcpy(send_buffer + 2, data + i, send_len);
			} else {
				send_len = len2 - i;
				memcpy(send_buffer + 2, data + i, send_len);
			}

			rf_tx_wrapper((char*)send_buffer, send_len + 2);
		}

		for (unsigned int i = end_a;i < len2;i += (MAX_PL_LEN - 3)) {
			uint8_t send_len = (MAX_PL_LEN - 3);
			send_buffer[0] = MOTE_PACKET_FILL_RX_BUFFER_LONG;
			send_buffer[1] = i >> 8;
			send_buffer[2] = i & 0xFF;

			if ((i + (MAX_PL_LEN - 3)) <= len2) {
				memcpy(send_buffer + 3, data + i, send_len);
			} else {
				send_len = len2 - i;
				memcpy(send_buffer + 3, data + i, send_len);
			}

			rf_tx_wrapper((char*)send_buffer, send_len + 3);
		}

		uint32_t ind = 0;
		send_buffer[ind++] = MOTE_PACKET_PROCESS_RX_BUFFER;
		send_buffer[ind++] = len >> 8;
		send_buffer[ind++] = len & 0xFF;
		unsigned short crc = crc16(data, len);
		send_buffer[ind++] = (uint8_t)(crc >> 8);
		send_buffer[ind++] = (uint8_t)(crc & 0xFF);
		memcpy(send_buffer + 5, data + len2, len - len2);
		ind += len - len2;

		rf_tx_wrapper((char*)send_buffer, ind);
	}
}




	// Packet interface
	packet_init(send_packet, send_buffer_nrf, 0);
	
	// NRF
	rf_init();
	rfhelp_init();
	print_rf_status();

	// Restart radio
	HAL_Delay(5);
	rfhelp_power_up();
	HAL_Delay(1);
	rfhelp_restart();

	rf_pair_and_set();



  int i = 0;
  uint8_t receiveBuffer[16] = { 0 };


  while (1) {

    // HAL_UART_Receive(&huart1, receiveBuffer, 16, 1024);
    // HAL_UART_Transmit(&huart1, "hello\n", 6, 1024);

    // SEGGER_RTT_WriteString(0, receiveBuffer);
    // SEGGER_RTT_WriteString(0, "\n");
    // SEGGER_RTT_printf(0, "%d\r\n", i);
    // //http://forum.segger.com/index.php?page=Thread&threadID=2881

    i+=1;

 
//HARDWARE-SPI-NRF
//http://www.edaboard.com/showthread.php?t=275093


	uint8_t pl[6];
	int32_t index = 0;
	pl[index++] = COMM_GET_VALUES;
	send_buffer_nrf(pl, index);

	// index = 0;
	// pl[index++] = MOTE_PACKET_PROCESS_SHORT_BUFFER;
	// pl[index++] = COMM_GET_VALUES;

	// rfhelp_power_up();
	// HAL_Delay(2);
	// rf_tx_wrapper((char*)pl, index);


	HAL_Delay(12);

		char buf[32];
		int len;
		int p;
		int ind;



		for(;;) {
#if NOACK
			int res = rfhelp_read_rx_data_crc_noack(buf, &len, &p);
#else
			int res = rfhelp_read_rx_data_crc(buf, &len, &p);
#endif
			// If something was read
			if (res >= 0) {
				// SEGGER_RTT_printf(0, "!!!GOT DATA!!!\n");
				MOTE_PACKET packet = buf[0];
				nrf_restart_rx_time = NRF_RESTART_TIMEOUT;

				switch (packet) {
				case MOTE_PACKET_BATT_LEVEL:
					// TODO!
					break;

				case MOTE_PACKET_ALIVE:
					// Everything triggers alive...
					SEGGER_RTT_printf(0, "GOT ALIVE PACKET!\n");
					break;

				case MOTE_PACKET_FILL_RX_BUFFER:
					memcpy(rx_buffer + buf[1], buf + 2, len - 2);
					break;

				case MOTE_PACKET_FILL_RX_BUFFER_LONG: {
					int rxbuf_ind = (unsigned int)buf[1] << 8;
					rxbuf_ind |= buf[2];
					if (rxbuf_ind < RX_BUFFER_SIZE) {
						memcpy(rx_buffer + rxbuf_ind, buf + 3, len - 3);
					}
				}
				break;

				case MOTE_PACKET_PROCESS_RX_BUFFER: {
					SEGGER_RTT_printf(0, "MOTE_PACKET_PROCESS_RX_BUFFER\n");
					ind = 1;
					int rxbuf_len = (unsigned int)buf[ind++] << 8;
					rxbuf_len |= (unsigned int)buf[ind++];

					if (rxbuf_len > RX_BUFFER_SIZE) {
						break;
					}

					uint8_t crc_high = buf[ind++];
					uint8_t crc_low = buf[ind++];

					memcpy(rx_buffer + rxbuf_len - (len - ind), buf + ind, len - ind);

					if (crc16(rx_buffer, rxbuf_len)
							== ((unsigned short) crc_high << 8
									| (unsigned short) crc_low)) {

						if (rx_buffer[0] == COMM_GET_VALUES) {
							nrf_stats.res_values++;

bldc_interface_process_packet(rx_buffer, rxbuf_len);

						}

						// Wait a bit in case retries are still made
						HAL_Delay(2);
						packet_send_packet(rx_buffer, rxbuf_len, 0);
					}
				}
				break;

				case MOTE_PACKET_PROCESS_SHORT_BUFFER:
					SEGGER_RTT_printf(0, "MOTE_PACKET_PROCESS_SHORT_BUFFER\n");
					// Wait a bit in case retries are still made
					HAL_Delay(2);
					packet_send_packet((unsigned char*)buf + 1, len - 1, 0);
					break;

				default:
					break;
				}
			}

			// Stop when there is no more data to read.
			if (res <= 0) {
				break;
			}
		}

	// HAL_Delay(10);



	print_rf_stats();
	print_rf_status();



char hall_value_conv_buffer[30] = { 0 };
itoa(SS495_ADC_buffer * 100 / 4096, hall_value_conv_buffer, 10); // SS495_ADC_buffer[512] * 100 / 4096 при 12 битах всего 4096 вариантов %
if(strlen(hall_value_conv_buffer) < 4) {
	strcat(hall_value_conv_buffer, " ");
}
SEGGER_RTT_printf(0, "%sadc values%s\n", RTT_CTRL_TEXT_BRIGHT_RED, RTT_CTRL_RESET);
SEGGER_RTT_printf(0, "%s\n", SS495_ADC_buffer);

SSD1306_GotoXY(20, 45); //Устанавливаем курсор в позицию 0;44. Сначала по горизонтали, потом вертикали.
SSD1306_Puts(hall_value_conv_buffer, &font_terminus_x20b, SSD1306_COLOR_WHITE); //пишем надпись в выставленной позиции шрифтом "Font_7x10" белым цветом. 
SSD1306_UpdateScreen();	



}//end-of-while(1)








}//end-of-main()


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) { }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
